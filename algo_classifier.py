"""
Classifying DFS against Fenwick-tree binaries with two pretrained backbones.

Runs on CPU. Set FAST_SANITY_CHECK = True for a short run that exercises the
whole pipeline before committing to the full one.

Usage:
    python algo_classifier.py

Expects:
    dataset/
        DFS/       *.png
        fenwick/   *.png
"""

import os

# Must be set before TensorFlow is imported to have any effect.
os.environ["TF_DETERMINISTIC_OPS"] = "1"
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "2"


import math
import random
import time
import traceback


import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import seaborn as sns
import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.applications import MobileNetV2, EfficientNetB0
from sklearn.metrics import confusion_matrix, classification_report


# ============================================================
# Configuration
# ============================================================


SEED = 42


DATASET_DIR = "dataset"
IMAGE_SIZE = (224, 224)
VAL_FRACTION = 0.2


BATCH_SIZE = 16
EPOCHS_HEAD = 80  # phase 1: backbone frozen, train the head
EPOCHS_FINETUNE = 30  # phase 2: unfreeze the top of the backbone
LR_HEAD = 5e-4
LR_FINETUNE = 5e-5  # 10x lower, so fine-tuning cannot wreck the features
UNFREEZE_FROM = 200  # backbone layers before this index stay frozen


PATIENCE_STOP = 10
PATIENCE_LR = 5


MODEL_TYPES = ["MobileNetV2", "EfficientNetB0"]
MODEL_COLORS = {"MobileNetV2": "#4C9BE8", "EfficientNetB0": "#5FBF77"}


OUTPUT_DIR = "results"


RUN_SINGLE_SPLIT = True
RUN_CROSS_VALIDATION = True


CV_FOLDS = 5
CV_EPOCHS_HEAD = 30
CV_EPOCHS_FINETUNE = 15

FAST_SANITY_CHECK = False


if FAST_SANITY_CHECK:
    EPOCHS_HEAD, EPOCHS_FINETUNE = 2, 1
    CV_EPOCHS_HEAD, CV_EPOCHS_FINETUNE = 2, 1
    CV_FOLDS = 2


random.seed(SEED)
np.random.seed(SEED)
tf.random.set_seed(SEED)


os.makedirs(OUTPUT_DIR, exist_ok=True)


def out(filename):
    """Path inside the output directory."""
    return os.path.join(OUTPUT_DIR, filename)


# ============================================================
# Data
# ============================================================


def build_split():
    """Build the train/validation split once and return file lists and labels.

    Everything downstream uses these lists and never re-reads the directory,
    so training and evaluation cannot disagree about which images are held out.
    """
    if not os.path.isdir(DATASET_DIR):
        raise SystemExit(
            f"Dataset directory '{DATASET_DIR}' not found.\n"
            f"Expected layout:\n  {DATASET_DIR}/DFS/*.png\n  {DATASET_DIR}/fenwick/*.png"
        )

    class_names = sorted(
        d
        for d in os.listdir(DATASET_DIR)
        if os.path.isdir(os.path.join(DATASET_DIR, d))
    )
    if len(class_names) < 2:
        raise SystemExit(
            f"Need at least 2 class folders in '{DATASET_DIR}', found {class_names}"
        )

    train_files, train_labels, val_files, val_labels = [], [], [], []
    counts = {}

    for label, class_name in enumerate(class_names):
        class_dir = os.path.join(DATASET_DIR, class_name)
        files = sorted(
            os.path.join(class_dir, f)
            for f in os.listdir(class_dir)
            if f.lower().endswith((".png", ".jpg", ".jpeg"))
        )
        counts[class_name] = len(files)
        if not files:
            raise SystemExit(f"No images found in {class_dir}")

        # sorted() then a seeded shuffle: identical split on every run, and
        # independent of the order the filesystem happens to return.
        rng = random.Random(SEED + label)
        rng.shuffle(files)

        n_val = max(1, int(round(len(files) * VAL_FRACTION)))
        val_files.extend(files[:n_val])
        val_labels.extend([label] * n_val)
        train_files.extend(files[n_val:])
        train_labels.extend([label] * (len(files) - n_val))

    print("\nDataset")
    print("-" * 60)
    for name in class_names:
        print(f"  {name:<12} {counts[name]:>4} images")
    print(f"  {'train':<12} {len(train_files):>4} images")
    print(f"  {'validation':<12} {len(val_files):>4} images")

    return class_names, train_files, train_labels, val_files, val_labels


def decode_image(path, label, num_classes):
    """Read one image as float32 in [0, 255].

    No scaling happens here; each model rescales its own input, so this
    pipeline stays neutral and cannot disagree with them.
    """
    raw = tf.io.read_file(path)
    img = tf.image.decode_image(raw, channels=3, expand_animations=False)
    img.set_shape([None, None, 3])
    img = tf.image.resize(img, IMAGE_SIZE)  # returns float32
    return img, tf.one_hot(label, depth=num_classes)


def build_augmentation():
    """Mild, structure-preserving augmentation.

    Deliberately absent:
      - flips: a binary image is a byte stream folded into a rectangle, so
        mirroring reorders the machine instructions
      - Gaussian noise: individual opcode bytes do not survive it
      - random crops: they delete whole subroutines
    """
    return tf.keras.Sequential(
        [
            layers.RandomRotation(0.02, fill_mode="constant", fill_value=255.0),
            layers.RandomZoom(0.06, fill_mode="constant", fill_value=255.0),
            layers.RandomTranslation(
                0.04, 0.04, fill_mode="constant", fill_value=255.0
            ),
            layers.RandomContrast(0.05),
        ],
        name="augmentation",
    )


def make_datasets(train_files, train_labels, val_files, val_labels, num_classes):
    """Build the tf.data pipelines.

    Ordering is cache -> shuffle -> batch -> augment: caching decodes and
    resizes each image once rather than every epoch, and augmenting after
    batching lets the preprocessing layers run vectorized over the batch.
    """
    augment = build_augmentation()

    train_ds = (
        tf.data.Dataset.from_tensor_slices((train_files, train_labels))
        .map(
            lambda p, l: decode_image(p, l, num_classes),
            num_parallel_calls=tf.data.AUTOTUNE,
        )
        .cache()
        .shuffle(len(train_files), seed=SEED, reshuffle_each_iteration=True)
        .batch(BATCH_SIZE)
        .map(
            lambda x, y: (augment(x, training=True), y),
            num_parallel_calls=tf.data.AUTOTUNE,
        )
        .prefetch(tf.data.AUTOTUNE)
    )

    # No augmentation and no shuffling on validation: it must be the same fixed
    # set every time or the numbers are not comparable across epochs.
    val_ds = (
        tf.data.Dataset.from_tensor_slices((val_files, val_labels))
        .map(
            lambda p, l: decode_image(p, l, num_classes),
            num_parallel_calls=tf.data.AUTOTUNE,
        )
        .cache()
        .batch(BATCH_SIZE)
        .prefetch(tf.data.AUTOTUNE)
    )

    return train_ds, val_ds


# ============================================================
# Models
# ============================================================


def build_model(model_type, num_classes):
    """Build a model that rescales its own input.

    MobileNetV2 expects [-1, 1]; Rescaling(1/127.5, -1) is what
    mobilenet_v2.preprocess_input computes. EfficientNetB0 expects raw
    [0, 255], since its Rescaling and Normalization layers are already part of
    the graph, so nothing is inserted.
    """
    inputs = tf.keras.Input(shape=IMAGE_SIZE + (3,), name="image_0_255")

    if model_type == "MobileNetV2":
        x = layers.Rescaling(scale=1.0 / 127.5, offset=-1.0, name="preprocess")(inputs)
        base = MobileNetV2(
            input_shape=IMAGE_SIZE + (3,), include_top=False, weights="imagenet"
        )
    elif model_type == "EfficientNetB0":
        x = inputs  # EfficientNet normalizes internally; do not touch the input
        base = EfficientNetB0(
            input_shape=IMAGE_SIZE + (3,), include_top=False, weights="imagenet"
        )
    else:
        raise ValueError(f"Unsupported model type: {model_type}")

    base.trainable = False

    # training=False keeps the backbone's BatchNorm layers in inference mode,
    # so their pretrained running statistics are used rather than recomputed
    # from these small batches.
    x = base(x, training=False)
    x = layers.GlobalAveragePooling2D()(x)
    x = layers.Dropout(0.3)(x)
    x = layers.Dense(
        128,
        kernel_regularizer=tf.keras.regularizers.l2(1e-4),
        name="head_dense_128",
    )(x)
    x = layers.BatchNormalization()(x)
    x = layers.Activation("relu")(x)
    x = layers.Dropout(0.4)(x)
    outputs = layers.Dense(num_classes, activation="softmax", name="predictions")(x)

    model = tf.keras.Model(inputs, outputs, name=f"{model_type}_classifier")
    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=LR_HEAD, clipnorm=1.0),
        loss="categorical_crossentropy",
        metrics=["accuracy"],
    )
    return model, base


def make_callbacks(model_type):
    """Early stopping and learning-rate reduction, both on validation loss."""
    return [
        tf.keras.callbacks.EarlyStopping(
            monitor="val_loss",
            patience=PATIENCE_STOP,
            restore_best_weights=True,
            verbose=1,
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor="val_loss",
            factor=0.5,
            patience=PATIENCE_LR,
            min_lr=1e-6,
            verbose=1,
        ),
    ]


def train_model(model_type, model, base, train_ds, val_ds):
    """Two-phase transfer learning."""
    print(f"\nPhase 1 -- training the head ({EPOCHS_HEAD} epochs, backbone frozen)")
    history_head = model.fit(
        train_ds,
        epochs=EPOCHS_HEAD,
        validation_data=val_ds,
        callbacks=make_callbacks(model_type),
        verbose=2,
    )

    print(
        f"\nPhase 2 -- fine-tuning from layer {UNFREEZE_FROM} ({EPOCHS_FINETUNE} epochs)"
    )
    base.trainable = True
    for i, layer in enumerate(base.layers):
        if i < UNFREEZE_FROM:
            layer.trainable = False
        # Backbone BatchNorm stays frozen regardless of depth: with this few
        # training images, recomputing the statistics costs more than the extra
        # capacity is worth.
        if isinstance(layer, layers.BatchNormalization):
            layer.trainable = False

    trainable = sum(1 for l in base.layers if l.trainable)
    print(
        f"  {trainable} of {len(base.layers)} backbone layers unfrozen "
        f"(BatchNorm layers kept frozen)"
    )

    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=LR_FINETUNE, clipnorm=1.0),
        loss="categorical_crossentropy",
        metrics=["accuracy"],
    )
    history_fine = model.fit(
        train_ds,
        epochs=EPOCHS_FINETUNE,
        validation_data=val_ds,
        callbacks=make_callbacks(model_type),
        verbose=2,
    )

    return history_head, history_fine


# ============================================================
# Evaluation
# ============================================================


def wilson_interval(correct, total, z=1.96):
    """95% Wilson confidence interval for a proportion.

    Reported because the evaluation sets are small: at n=20 the interval is
    roughly +/- 20 percentage points.
    """
    if total == 0:
        return 0.0, 0.0
    p = correct / total
    denom = 1 + z * z / total
    center = (p + z * z / (2 * total)) / denom
    half = (z / denom) * math.sqrt(p * (1 - p) / total + z * z / (4 * total * total))
    return max(0.0, center - half), min(1.0, center + half)


def evaluate(model_type, model, files, labels, class_names, split_name):
    """Score a file list in a single batched pass."""
    num_classes = len(class_names)
    ds = (
        tf.data.Dataset.from_tensor_slices((files, labels))
        .map(
            lambda p, l: decode_image(p, l, num_classes),
            num_parallel_calls=tf.data.AUTOTUNE,
        )
        .batch(BATCH_SIZE)
        .prefetch(tf.data.AUTOTUNE)
    )

    probs = model.predict(ds, verbose=0)
    y_pred = np.argmax(probs, axis=1)
    y_true = np.array(labels)
    confidence = probs.max(axis=1)

    correct = int((y_pred == y_true).sum())
    total = len(y_true)
    accuracy = correct / total
    lo, hi = wilson_interval(correct, total)

    print(f"\n{model_type} -- {split_name} set ({total} images)")
    print("-" * 60)
    print(f"  accuracy        {accuracy:.2%}   ({correct}/{total})")
    print(f"  95% CI          [{lo:.2%}, {hi:.2%}]")
    print(f"  mean confidence {confidence.mean():.2%}")

    per_class = {}
    for i, name in enumerate(class_names):
        mask = y_true == i
        per_class[name] = float((y_pred[mask] == i).mean()) if mask.any() else 0.0
        print(f"  recall {name:<12} {per_class[name]:.2%}")

    print()
    print(
        classification_report(y_true, y_pred, target_names=class_names, zero_division=0)
    )

    return {
        "accuracy": accuracy,
        "ci": (lo, hi),
        "per_class": per_class,
        "y_true": y_true,
        "y_pred": y_pred,
        "confidence": confidence,
    }


# ============================================================
# Plots
# ============================================================


def plot_augmentation_preview(train_files, class_names):
    """Show one training image and several augmented copies of it.

    Checks that the byte-level structure carrying the label survives
    augmentation.
    """
    augment = build_augmentation()
    path = train_files[0]
    raw, _ = decode_image(tf.constant(path), 0, len(class_names))
    batch = tf.repeat(raw[tf.newaxis, ...], 7, axis=0)
    augmented = augment(batch, training=True).numpy()

    fig, axes = plt.subplots(2, 4, figsize=(13, 7))
    axes = axes.ravel()
    axes[0].imshow(raw.numpy().astype("uint8"))
    axes[0].set_title("original", fontsize=10, fontweight="bold")
    axes[0].axis("off")
    for i in range(7):
        axes[i + 1].imshow(np.clip(augmented[i], 0, 255).astype("uint8"))
        axes[i + 1].set_title(f"augmented {i + 1}", fontsize=10)
        axes[i + 1].axis("off")
    fig.suptitle(
        "Augmentation preview -- the structure must stay readable", fontsize=13
    )
    fig.tight_layout()
    fig.savefig(out("augmentation_preview.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out('augmentation_preview.png')}")


def plot_training_curves(model_type, history_head, history_fine):
    """Accuracy and loss across both phases, with the phase boundary marked."""

    def joined(key):
        return list(history_head.history[key]) + list(history_fine.history[key])

    boundary = len(history_head.history["accuracy"])
    epochs = range(1, boundary + len(history_fine.history["accuracy"]) + 1)

    fig, axes = plt.subplots(1, 2, figsize=(13, 5))
    for ax, (train_key, val_key, title) in zip(
        axes,
        [("accuracy", "val_accuracy", "Accuracy"), ("loss", "val_loss", "Loss")],
    ):
        ax.plot(
            epochs, joined(train_key), label="train", color=MODEL_COLORS[model_type]
        )
        ax.plot(epochs, joined(val_key), label="validation", color="#D95F5F")
        ax.axvline(boundary + 0.5, color="grey", linestyle=":", linewidth=1.5)
        ax.annotate(
            "fine-tuning starts",
            xy=(boundary + 0.5, ax.get_ylim()[1]),
            xytext=(4, -12),
            textcoords="offset points",
            fontsize=9,
            color="grey",
        )
        ax.set_title(f"{model_type} -- {title}")
        ax.set_xlabel("epoch")
        ax.set_ylabel(title.lower())
        ax.grid(True, linestyle="--", alpha=0.5)
        ax.legend()
        if title == "Accuracy":
            ax.axhline(0.5, color="black", linestyle="--", linewidth=1, alpha=0.4)
            ax.set_ylim(0, 1.02)

    fig.tight_layout()
    fig.savefig(out(f"training_curves_{model_type}.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out(f'training_curves_{model_type}.png')}")


def plot_confusion(model_type, result, class_names, split_name):
    """Counts and row-normalized recall side by side.

    The normalized panel reveals a model that has collapsed onto one label,
    which a raw accuracy figure hides.
    """
    cm = confusion_matrix(
        result["y_true"], result["y_pred"], labels=range(len(class_names))
    )
    cm_norm = cm / np.maximum(cm.sum(axis=1, keepdims=True), 1)

    fig, axes = plt.subplots(1, 2, figsize=(13, 5))
    sns.heatmap(
        cm,
        annot=True,
        fmt="d",
        cmap="Blues",
        cbar=False,
        xticklabels=class_names,
        yticklabels=class_names,
        ax=axes[0],
    )
    axes[0].set_title("counts")
    sns.heatmap(
        cm_norm,
        annot=True,
        fmt=".2f",
        cmap="Blues",
        vmin=0,
        vmax=1,
        cbar=False,
        xticklabels=class_names,
        yticklabels=class_names,
        ax=axes[1],
    )
    axes[1].set_title("normalized by true class (recall on the diagonal)")
    for ax in axes:
        ax.set_xlabel("predicted")
        ax.set_ylabel("true")

    fig.suptitle(f"{model_type} -- {split_name} set", fontsize=13)
    fig.tight_layout()
    fig.savefig(out(f"confusion_{model_type}_{split_name}.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out(f'confusion_{model_type}_{split_name}.png')}")


def plot_comparison(results, class_names):
    """Validation and training accuracy, per-class recall, and training time."""
    models = [m for m in MODEL_TYPES if m in results]
    if not models:
        return

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))

    # Validation accuracy with confidence intervals.
    accs = [results[m]["val"]["accuracy"] for m in models]
    errs = np.array(
        [
            [a - results[m]["val"]["ci"][0] for a, m in zip(accs, models)],
            [results[m]["val"]["ci"][1] - a for a, m in zip(accs, models)],
        ]
    )
    axes[0, 0].bar(
        models,
        accs,
        color=[MODEL_COLORS[m] for m in models],
        yerr=errs,
        capsize=6,
        ecolor="#444444",
    )
    axes[0, 0].axhline(0.5, color="black", linestyle="--", linewidth=1.2)
    axes[0, 0].text(len(models) - 0.5, 0.515, "chance", fontsize=9, ha="right")
    axes[0, 0].set_ylim(0, 1.05)
    axes[0, 0].set_ylabel("accuracy")
    axes[0, 0].set_title("Validation accuracy (held out, with 95% CI)")
    for i, v in enumerate(accs):
        axes[0, 0].text(i, v + 0.03, f"{v:.1%}", ha="center", fontweight="bold")

    # Training accuracy alongside it, so the gap between them is visible.
    x = np.arange(len(models))
    width = 0.35
    axes[0, 1].bar(
        x - width / 2,
        [results[m]["train"]["accuracy"] for m in models],
        width,
        label="train",
        color="#B8B8B8",
    )
    axes[0, 1].bar(
        x + width / 2,
        accs,
        width,
        label="validation",
        color=[MODEL_COLORS[m] for m in models],
    )
    axes[0, 1].axhline(0.5, color="black", linestyle="--", linewidth=1.2)
    axes[0, 1].set_xticks(x)
    axes[0, 1].set_xticklabels(models)
    axes[0, 1].set_ylim(0, 1.05)
    axes[0, 1].set_title("Train vs. validation")
    axes[0, 1].legend()

    # Per-class recall on validation.
    xc = np.arange(len(class_names))
    for i, m in enumerate(models):
        vals = [results[m]["val"]["per_class"].get(c, 0) for c in class_names]
        axes[1, 0].bar(
            xc + (i - (len(models) - 1) / 2) * width,
            vals,
            width,
            label=m,
            color=MODEL_COLORS[m],
        )
    axes[1, 0].axhline(0.5, color="black", linestyle="--", linewidth=1.2)
    axes[1, 0].set_xticks(xc)
    axes[1, 0].set_xticklabels(class_names)
    axes[1, 0].set_ylim(0, 1.05)
    axes[1, 0].set_ylabel("recall")
    axes[1, 0].set_title("Per-class recall (validation)")
    axes[1, 0].legend()

    # Training time.
    times = [results[m]["training_time"] for m in models]
    axes[1, 1].bar(models, times, color=[MODEL_COLORS[m] for m in models])
    axes[1, 1].set_ylabel("seconds")
    axes[1, 1].set_title("Training time (CPU)")
    for i, v in enumerate(times):
        axes[1, 1].text(i, v + max(times) * 0.02, f"{v:.0f}s", ha="center")

    for ax in axes.ravel():
        ax.grid(True, axis="y", linestyle="--", alpha=0.4)
        ax.set_axisbelow(True)

    fig.tight_layout()
    fig.savefig(out("model_comparison.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out('model_comparison.png')}")


def plot_preprocessing_diagnostic(sample_file):
    """Compare EfficientNetB0 features under correct and naive input scaling.

    Passes one image through the untrained backbone twice, once as raw
    [0, 255] as the model expects and once divided by 255, and compares the
    resulting feature vectors. Quantifies what the reflex normalization of most
    image pipelines costs, without training anything.
    """
    print("\nPreprocessing diagnostic (no training involved)")
    print("-" * 60)

    base = EfficientNetB0(
        input_shape=IMAGE_SIZE + (3,), include_top=False, weights="imagenet"
    )
    pool = tf.keras.Sequential([base, layers.GlobalAveragePooling2D()])

    raw, _ = decode_image(tf.constant(sample_file), 0, 2)
    raw = raw[tf.newaxis, ...]

    correct = pool(raw, training=False).numpy().ravel()
    naive = pool(raw / 255.0, training=False).numpy().ravel()

    cos = float(
        np.dot(correct, naive)
        / (np.linalg.norm(correct) * np.linalg.norm(naive) + 1e-12)
    )
    print(
        f"  correct input [0,255]:  mean {correct.mean():.4f}   std {correct.std():.4f}"
    )
    print(f"  naive   input [0,1]  :  mean {naive.mean():.4f}   std {naive.std():.4f}")
    print(f"  cosine similarity between the two feature vectors: {cos:.4f}")
    print("  (1.0 would mean the scaling made no difference)")

    fig, axes = plt.subplots(1, 2, figsize=(13, 5))
    axes[0].hist(correct, bins=60, color="#4C9BE8", alpha=0.85)
    axes[0].set_title("EfficientNetB0 features -- correct input [0, 255]")
    axes[1].hist(naive, bins=60, color="#D95F5F", alpha=0.85)
    axes[1].set_title("EfficientNetB0 features -- input scaled to [0, 1]")
    for ax in axes:
        ax.set_xlabel("activation value")
        ax.set_ylabel("count")
        ax.grid(True, linestyle="--", alpha=0.4)
    fig.suptitle(
        f"Same image, same weights, different input scaling "
        f"(cosine similarity = {cos:.3f})",
        fontsize=13,
    )
    fig.tight_layout()
    fig.savefig(out("preprocessing_diagnostic.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out('preprocessing_diagnostic.png')}")


# ============================================================
# Cross-validation
# ============================================================


def build_folds(n_splits):
    """Stratified k-fold assignment over the whole dataset.

    Each class is shuffled under a fixed seed and dealt round-robin into the
    folds, so every fold holds the same number of images from each class. Every
    image is then validated exactly once, by a model that did not train on it.
    """
    class_names = sorted(
        d
        for d in os.listdir(DATASET_DIR)
        if os.path.isdir(os.path.join(DATASET_DIR, d))
    )
    folds = [[] for _ in range(n_splits)]

    for label, class_name in enumerate(class_names):
        class_dir = os.path.join(DATASET_DIR, class_name)
        files = sorted(
            os.path.join(class_dir, f)
            for f in os.listdir(class_dir)
            if f.lower().endswith((".png", ".jpg", ".jpeg"))
        )
        rng = random.Random(SEED + label)
        rng.shuffle(files)
        for i, path in enumerate(files):
            folds[i % n_splits].append((path, label))

    return class_names, folds


def binomial_p_value(correct, total):
    """One-sided P(X >= correct) under X ~ Binomial(total, 0.5).

    The probability of scoring this well or better by guessing.
    """
    if total == 0:
        return 1.0
    tail = sum(math.comb(total, k) for k in range(correct, total + 1))
    return tail / (2**total)


def train_one_fold(model_type, train_pairs, val_pairs, num_classes):
    """Train one fold on a fixed schedule and return its history and predictions.

    No callbacks are used here. Early stopping with weight restoration selects
    the reported weights using the same images that then score them, which on a
    twenty-image fold is a substantial source of optimistic bias. Each fold
    therefore runs a fixed number of epochs at a fixed learning rate, and the
    final-epoch weights are the ones evaluated.
    """
    train_files = [p for p, _ in train_pairs]
    train_labels = [l for _, l in train_pairs]
    val_files = [p for p, _ in val_pairs]
    val_labels = [l for _, l in val_pairs]

    train_ds, val_ds = make_datasets(
        train_files, train_labels, val_files, val_labels, num_classes
    )

    model, base = build_model(model_type, num_classes)

    history_head = model.fit(
        train_ds, epochs=CV_EPOCHS_HEAD, validation_data=val_ds, verbose=0
    )

    base.trainable = True
    for i, layer in enumerate(base.layers):
        if i < UNFREEZE_FROM:
            layer.trainable = False
        if isinstance(layer, layers.BatchNormalization):
            layer.trainable = False

    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=LR_FINETUNE, clipnorm=1.0),
        loss="categorical_crossentropy",
        metrics=["accuracy"],
    )
    history_fine = model.fit(
        train_ds, epochs=CV_EPOCHS_FINETUNE, validation_data=val_ds, verbose=0
    )

    probs = model.predict(
        tf.data.Dataset.from_tensor_slices((val_files, val_labels))
        .map(
            lambda p, l: decode_image(p, l, num_classes),
            num_parallel_calls=tf.data.AUTOTUNE,
        )
        .batch(BATCH_SIZE),
        verbose=0,
    )
    y_pred = np.argmax(probs, axis=1)
    y_true = np.array(val_labels)

    history = {
        key: list(history_head.history[key]) + list(history_fine.history[key])
        for key in history_head.history
        if key in history_fine.history
    }

    del model, base
    tf.keras.backend.clear_session()

    return history, y_true, y_pred


def run_cross_validation(class_names, folds):
    """Run k-fold CV for every model and return pooled out-of-fold results."""
    num_classes = len(class_names)
    cv_results = {}

    for model_type in MODEL_TYPES:
        print("\n" + "=" * 60)
        print(f"  {CV_FOLDS}-FOLD CROSS-VALIDATION -- {model_type}")
        print("=" * 60)
        print(
            f"  fixed schedule: {CV_EPOCHS_HEAD} head epochs + "
            f"{CV_EPOCHS_FINETUNE} fine-tune epochs, no callbacks"
        )

        histories, fold_accs = [], []
        pooled_true, pooled_pred = [], []
        start = time.time()

        for k in range(CV_FOLDS):
            val_pairs = folds[k]
            train_pairs = [pair for j, f in enumerate(folds) if j != k for pair in f]

            history, y_true, y_pred = train_one_fold(
                model_type, train_pairs, val_pairs, num_classes
            )

            acc = float((y_pred == y_true).mean())
            fold_accs.append(acc)
            histories.append(history)
            pooled_true.extend(y_true.tolist())
            pooled_pred.extend(y_pred.tolist())

            print(
                f"  fold {k + 1}/{CV_FOLDS}:  "
                f"{int((y_pred == y_true).sum())}/{len(y_true)} = {acc:.1%}"
            )

        elapsed = time.time() - start
        pooled_true = np.array(pooled_true)
        pooled_pred = np.array(pooled_pred)

        correct = int((pooled_pred == pooled_true).sum())
        total = len(pooled_true)
        pooled_acc = correct / total
        lo, hi = wilson_interval(correct, total)
        p_value = binomial_p_value(correct, total)

        per_class = {}
        for i, name in enumerate(class_names):
            mask = pooled_true == i
            per_class[name] = (
                float((pooled_pred[mask] == i).mean()) if mask.any() else 0.0
            )

        print(f"\n  pooled out-of-fold: {correct}/{total} = {pooled_acc:.1%}")
        print(f"  95% CI              [{lo:.1%}, {hi:.1%}]")
        print(
            f"  fold mean +/- std   {np.mean(fold_accs):.1%} +/- {np.std(fold_accs):.1%}"
        )
        print(f"  fold range          {min(fold_accs):.1%} to {max(fold_accs):.1%}")
        print(f"  p vs chance         {p_value:.4f}")
        for name, acc in per_class.items():
            print(f"  recall {name:<12} {acc:.1%}")
        print(f"  total time          {elapsed:.0f}s")
        print()
        print(
            classification_report(
                pooled_true, pooled_pred, target_names=class_names, zero_division=0
            )
        )

        cv_results[model_type] = {
            "fold_accs": fold_accs,
            "histories": histories,
            "pooled_true": pooled_true,
            "pooled_pred": pooled_pred,
            "pooled_acc": pooled_acc,
            "ci": (lo, hi),
            "p_value": p_value,
            "per_class": per_class,
            "time": elapsed,
        }

        plot_cv_epoch_curves(model_type, histories)

    return cv_results


def plot_cv_epoch_curves(model_type, histories):
    """Accuracy and loss per epoch, one line per fold plus the mean.

    Shows how much of a single run is luck: tightly bunched fold curves mean a
    stable procedure, a wide fan means any one holdout number says little.
    """
    n_epochs = min(len(h["val_accuracy"]) for h in histories)
    epochs = np.arange(1, n_epochs + 1)
    boundary = CV_EPOCHS_HEAD

    fig, axes = plt.subplots(1, 2, figsize=(14, 5.5))
    color = MODEL_COLORS[model_type]

    for ax, (train_key, val_key, label) in zip(
        axes,
        [("accuracy", "val_accuracy", "accuracy"), ("loss", "val_loss", "loss")],
    ):
        val_curves = np.array([h[val_key][:n_epochs] for h in histories])
        train_curves = np.array([h[train_key][:n_epochs] for h in histories])

        for i, curve in enumerate(val_curves):
            ax.plot(
                epochs,
                curve,
                color=color,
                alpha=0.28,
                linewidth=1.2,
                label="individual folds" if i == 0 else None,
            )

        ax.plot(
            epochs,
            val_curves.mean(axis=0),
            color=color,
            linewidth=2.8,
            label=f"validation (mean of {len(histories)} folds)",
        )
        ax.plot(
            epochs,
            train_curves.mean(axis=0),
            color="#999999",
            linewidth=2.0,
            linestyle="--",
            label="train (mean)",
        )

        mean, std = val_curves.mean(axis=0), val_curves.std(axis=0)
        ax.fill_between(
            epochs,
            mean - std,
            mean + std,
            color=color,
            alpha=0.13,
            label="+/- 1 sd across folds",
        )

        ax.axvline(boundary + 0.5, color="grey", linestyle=":", linewidth=1.5)
        ax.set_xlabel("epoch")
        ax.set_ylabel(label)
        ax.set_title(f"{model_type} -- validation {label} across folds")
        ax.grid(True, linestyle="--", alpha=0.45)
        ax.set_axisbelow(True)
        ax.legend(fontsize=8, loc="best")

        if label == "accuracy":
            ax.axhline(0.5, color="black", linestyle="--", linewidth=1.1, alpha=0.55)
            ax.text(1, 0.505, "chance", fontsize=8, color="black", alpha=0.7)
            ax.set_ylim(0, 1.02)

    axes[0].annotate(
        "fine-tuning starts",
        xy=(boundary + 0.5, 0.04),
        xytext=(5, 0),
        textcoords="offset points",
        fontsize=9,
        color="grey",
    )

    fig.suptitle(
        f"{model_type} -- {len(histories)}-fold cross-validation, fixed schedule",
        fontsize=13,
    )
    fig.tight_layout()
    fig.savefig(out(f"cv_epoch_curves_{model_type}.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out(f'cv_epoch_curves_{model_type}.png')}")


def plot_cv_summary(cv_results, class_names):
    """Per-fold spread, pooled accuracy, and the pooled confusion matrices."""
    models = [m for m in MODEL_TYPES if m in cv_results]
    if not models:
        return

    fig = plt.figure(figsize=(14, 10))
    gs = fig.add_gridspec(2, len(models), height_ratios=[1, 1])

    # Top left: per-fold accuracy, one point per fold.
    ax = fig.add_subplot(gs[0, 0])
    for i, m in enumerate(models):
        accs = cv_results[m]["fold_accs"]
        jitter = (np.random.RandomState(SEED).rand(len(accs)) - 0.5) * 0.12
        ax.scatter(
            np.full(len(accs), i) + jitter,
            accs,
            s=55,
            color=MODEL_COLORS[m],
            alpha=0.75,
            zorder=3,
            label="folds" if i == 0 else None,
        )
        ax.hlines(
            cv_results[m]["pooled_acc"],
            i - 0.28,
            i + 0.28,
            color="black",
            linewidth=2.4,
            zorder=4,
            label="pooled" if i == 0 else None,
        )
    ax.axhline(0.5, color="black", linestyle="--", linewidth=1.1, alpha=0.55)
    ax.set_xticks(range(len(models)))
    ax.set_xticklabels(models)
    ax.set_ylim(0, 1.05)
    ax.set_ylabel("accuracy")
    ax.set_title("Per-fold accuracy (spread = how lucky one split can be)")
    ax.grid(True, axis="y", linestyle="--", alpha=0.45)
    ax.set_axisbelow(True)
    ax.legend(fontsize=8)

    # Top right: pooled accuracy with CI.
    ax = fig.add_subplot(gs[0, -1])
    accs = [cv_results[m]["pooled_acc"] for m in models]
    errs = np.array(
        [
            [a - cv_results[m]["ci"][0] for a, m in zip(accs, models)],
            [cv_results[m]["ci"][1] - a for a, m in zip(accs, models)],
        ]
    )
    ax.bar(
        models,
        accs,
        color=[MODEL_COLORS[m] for m in models],
        yerr=errs,
        capsize=6,
        ecolor="#444444",
    )
    ax.axhline(0.5, color="black", linestyle="--", linewidth=1.2)
    ax.set_ylim(0, 1.05)
    ax.set_ylabel("accuracy")
    ax.set_title("Pooled out-of-fold accuracy, all 100 images (95% CI)")
    for i, (m, v) in enumerate(zip(models, accs)):
        ax.text(
            i,
            v + 0.03,
            f"{v:.1%}\np={cv_results[m]['p_value']:.3f}",
            ha="center",
            fontweight="bold",
            fontsize=9,
        )
    ax.grid(True, axis="y", linestyle="--", alpha=0.45)
    ax.set_axisbelow(True)

    # Bottom: pooled confusion matrix per model.
    for i, m in enumerate(models):
        ax = fig.add_subplot(gs[1, i])
        cm = confusion_matrix(
            cv_results[m]["pooled_true"],
            cv_results[m]["pooled_pred"],
            labels=range(len(class_names)),
        )
        cm_norm = cm / np.maximum(cm.sum(axis=1, keepdims=True), 1)
        annot = np.array(
            [
                [f"{cm[r][c]}\n{cm_norm[r][c]:.0%}" for c in range(len(class_names))]
                for r in range(len(class_names))
            ]
        )
        sns.heatmap(
            cm_norm,
            annot=annot,
            fmt="",
            cmap="Blues",
            vmin=0,
            vmax=1,
            cbar=False,
            xticklabels=class_names,
            yticklabels=class_names,
            ax=ax,
        )
        ax.set_title(f"{m} -- pooled out-of-fold")
        ax.set_xlabel("predicted")
        ax.set_ylabel("true")

    fig.suptitle(
        f"{CV_FOLDS}-fold cross-validation -- every image validated exactly once",
        fontsize=14,
    )
    fig.tight_layout()
    fig.savefig(out("cv_summary.png"), dpi=130)
    plt.close(fig)
    print(f"  wrote {out('cv_summary.png')}")


# ============================================================
# Main
# ============================================================


def main():
    class_names, train_files, train_labels, val_files, val_labels = build_split()
    num_classes = len(class_names)
    train_ds, val_ds = make_datasets(
        train_files, train_labels, val_files, val_labels, num_classes
    )

    steps = math.ceil(len(train_files) / BATCH_SIZE)
    print(
        f"\n  {steps} steps/epoch x {EPOCHS_HEAD + EPOCHS_FINETUNE} epochs "
        f"= {steps * (EPOCHS_HEAD + EPOCHS_FINETUNE)} optimizer steps"
    )

    print("\nWriting diagnostic plots")
    print("-" * 60)
    plot_augmentation_preview(train_files, class_names)
    plot_preprocessing_diagnostic(train_files[0])

    results = {}
    for model_type in MODEL_TYPES if RUN_SINGLE_SPLIT else []:
        try:
            print("\n" + "=" * 60)
            print(f"  SINGLE SPLIT -- {model_type}")
            print("=" * 60)

            model, base = build_model(model_type, num_classes)

            start = time.time()
            history_head, history_fine = train_model(
                model_type, model, base, train_ds, val_ds
            )
            elapsed = time.time() - start
            print(f"\n  training time: {elapsed:.1f}s")

            val_result = evaluate(
                model_type, model, val_files, val_labels, class_names, "validation"
            )
            train_result = evaluate(
                model_type, model, train_files, train_labels, class_names, "train"
            )

            results[model_type] = {
                "val": val_result,
                "train": train_result,
                "training_time": elapsed,
            }

            print()
            plot_training_curves(model_type, history_head, history_fine)
            plot_confusion(model_type, val_result, class_names, "validation")

            model.save_weights(out(f"{model_type}.weights.h5"))
            print(f"  wrote {out(f'{model_type}.weights.h5')}")

        except Exception as exc:
            print(f"\nError while processing {model_type}: {exc}")
            traceback.print_exc()

    if results:
        print("\n" + "=" * 60)
        print("  SINGLE-SPLIT SUMMARY (20 validation images)")
        print("=" * 60)
        for model_type, r in results.items():
            lo, hi = r["val"]["ci"]
            print(f"\n  {model_type}")
            print(
                f"    validation accuracy  {r['val']['accuracy']:.2%}  [{lo:.1%}, {hi:.1%}]"
            )
            print(f"    train accuracy       {r['train']['accuracy']:.2%}")
            print(f"    training time        {r['training_time']:.1f}s")
            for name, acc in r["val"]["per_class"].items():
                print(f"    recall {name:<14} {acc:.2%}")

        print("\n  Note: these use EarlyStopping(restore_best_weights=True), which")
        print("  selects weights using these same 20 images. Treat them as slightly")
        print("  optimistic. The cross-validation numbers below do not have that bias.")

        print()
        plot_comparison(results, class_names)

    if RUN_CROSS_VALIDATION:
        cv_class_names, folds = build_folds(CV_FOLDS)
        sizes = ", ".join(str(len(f)) for f in folds)
        print("\n" + "=" * 60)
        print(f"  {CV_FOLDS}-FOLD CROSS-VALIDATION")
        print("=" * 60)
        print(f"  fold sizes: {sizes}  (stratified: equal classes per fold)")
        print(f"  every image is validated exactly once, by a model that never saw it")
        print(f"  this trains {CV_FOLDS * len(MODEL_TYPES)} models")

        try:
            cv_results = run_cross_validation(cv_class_names, folds)
            print()
            plot_cv_summary(cv_results, cv_class_names)

            print("\n" + "=" * 60)
            print("  CROSS-VALIDATED SUMMARY (all 100 images, no contamination)")
            print("=" * 60)
            for model_type, r in cv_results.items():
                lo, hi = r["ci"]
                print(f"\n  {model_type}")
                print(
                    f"    pooled accuracy   {r['pooled_acc']:.1%}  [{lo:.1%}, {hi:.1%}]"
                )
                print(
                    f"    fold mean +/- sd  {np.mean(r['fold_accs']):.1%} "
                    f"+/- {np.std(r['fold_accs']):.1%}"
                )
                print(
                    f"    folds             "
                    + "  ".join(f"{a:.0%}" for a in r["fold_accs"])
                )
                print(
                    f"    p vs chance       {r['p_value']:.4f}"
                    + (
                        "  (significant)"
                        if r["p_value"] < 0.05
                        else "  (not significant)"
                    )
                )
                for name, acc in r["per_class"].items():
                    print(f"    recall {name:<12} {acc:.1%}")

            print("\n  Chance is 50%. A p-value below 0.05 means the model is doing")
            print("  something real; a wide fold spread means one 20-image split")
            print("  could easily have told you the opposite story.")

        except Exception as exc:
            print(f"\nCross-validation failed: {exc}")
            traceback.print_exc()

    print(f"\nAll outputs written to {OUTPUT_DIR}/")


if __name__ == "__main__":
    main()
