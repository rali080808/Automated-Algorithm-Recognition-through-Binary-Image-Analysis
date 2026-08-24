import os
import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.keras import layers, models
from tensorflow.keras.applications import MobileNetV2, EfficientNetB0, ResNet50V2
import random
import time
from sklearn.metrics import confusion_matrix, classification_report
import seaborn as sns
import traceback
SEED = 42
random.seed(SEED)
np.random.seed(SEED)
tf.random.set_seed(SEED)
os.environ['TF_DETERMINISTIC_OPS'] = '1'
print("TensorFlow version:", tf.__version__)
print("Eager execution enabled:", tf.executing_eagerly())
if not tf.executing_eagerly():
    print("Enabling eager execution...")
    tf.compat.v1.enable_eager_execution()
IMAGE_SIZE = (224, 224)
BATCH_SIZE = 4            
EPOCHS1 = 5       
EPOCHS2 = 5
PATIENCE = 20             
LEARNING_RATE = 0.0005    
DATASET_DIR = "dataset"
COUNT_DFS_IMAGES = 50
COUNT_FENWICK_IMAGES = 50
MODEL_TYPES = ["MobileNetV2", "EfficientNetB0"]
MODEL_COLORS = {
    "MobileNetV2": "skyblue",
    "EfficientNetB0": "lightgreen"
}
model_results = {
    "MobileNetV2": {
        "model": None,
        "base_model": None,
        "val_accuracy": 0,
        "class_accuracy": {},
        "per_style_accuracy": {},
        "training_time": 0,
        "history": None,
        "history_fine": None
    },
    "EfficientNetB0": {
        "model": None,
        "base_model": None,
        "val_accuracy": 0,
        "class_accuracy": {},
        "per_style_accuracy": {},
        "training_time": 0,
        "history": None,
        "history_fine": None
    }
}
def analyze_dataset():
    class_counts = {}
    class_files = {}
    for class_name in os.listdir(DATASET_DIR):
        class_path = os.path.join(DATASET_DIR, class_name)
        if os.path.isdir(class_path):
            image_files = [
                f for f in os.listdir(class_path) 
                if f.lower().endswith(('.png', '.jpg', '.jpeg'))
            ]
            class_counts[class_name] = len(image_files)
            class_files[class_name] = [os.path.join(class_path, f) for f in image_files]
    class_names = sorted(list(class_counts.keys()))
    class_indices = {name: i for i, name in enumerate(class_names)}
    print("\nDataset Analysis:")
    print(f"Total classes: {len(class_names)}")
    print("\nClass distribution:")
    for cls, count in class_counts.items():
        print(f"  {cls}: {count} images")
    max_count = max(class_counts.values())
    weight_dict = {}
    for i, cls in enumerate(class_names):
        weight_dict[i] = max_count / class_counts[cls] if class_counts[cls] > 0 else 1.0
    print("\nClass weights for balanced training:")
    for i, cls in enumerate(class_names):
        print(f"  {cls} (index {i}): {weight_dict[i]:.2f}")
    return class_names, class_indices, weight_dict, class_files
def create_custom_dataset():
    class_names, class_indices, class_weights, class_files = analyze_dataset()
    num_classes = len(class_names)
    train_files, train_labels, val_files, val_labels = [], [], [], []
    for class_name, files in class_files.items():
        random.shuffle(files)
        split_idx = max(1, int(len(files) * 0.8))  
        train_files.extend(files[:split_idx])
        train_labels.extend([class_indices[class_name]] * split_idx)
        val_files.extend(files[split_idx:])
        val_labels.extend([class_indices[class_name]] * (len(files) - split_idx))
    print(f"\nTrain set: {len(train_files)} images")
    print(f"Validation set: {len(val_files)} images")
    data_augmentation = tf.keras.Sequential([
        layers.RandomFlip("horizontal_and_vertical"),
        layers.RandomRotation(0.3, fill_mode='reflect'),
        layers.RandomZoom(0.4, fill_mode='reflect'),
        layers.RandomContrast(0.4),
        layers.RandomBrightness(0.3),
        layers.RandomTranslation(0.3, 0.3, fill_mode='reflect'),
        layers.GaussianNoise(0.2),  
        layers.RandomCrop(180, 180),
        layers.Resizing(IMAGE_SIZE[0], IMAGE_SIZE[1])
    ])
    def load_and_preprocess_image(file_path, label, is_training=True):
        img = tf.io.read_file(file_path)
        img = tf.image.decode_png(img, channels=3)
        img = tf.image.resize(img, IMAGE_SIZE)
        img = tf.cast(img, tf.float32) / 255.0
        if is_training and tf.random.uniform([], 0, 1) < 0.9:
            img = data_augmentation(img)
        label = tf.one_hot(label, depth=num_classes)
        return img, label
    train_dataset = tf.data.Dataset.from_tensor_slices((train_files, train_labels))
    train_dataset = train_dataset.map(
        lambda x, y: load_and_preprocess_image(x, y, is_training=True),
        num_parallel_calls=tf.data.AUTOTUNE
    ).repeat(10).shuffle(buffer_size=1000).batch(BATCH_SIZE).prefetch(tf.data.AUTOTUNE)
    val_dataset = tf.data.Dataset.from_tensor_slices((val_files, val_labels))
    val_dataset = val_dataset.map(
        lambda x, y: load_and_preprocess_image(x, y, is_training=False),
        num_parallel_calls=tf.data.AUTOTUNE
    ).batch(BATCH_SIZE).prefetch(tf.data.AUTOTUNE)
    return train_dataset, val_dataset, class_names, num_classes, class_weights, train_files, val_files, train_labels, val_labels
print("\nPreparing custom dataset with heavy augmentation...")
train_dataset, val_dataset, class_names, num_classes, class_weights, train_files, val_files, train_labels, val_labels = create_custom_dataset()
def create_standard_model(base_model, num_classes):
    base_model.trainable = False
    model = models.Sequential([
        layers.InputLayer(input_shape=IMAGE_SIZE + (3,)),
        base_model,
        layers.GlobalAveragePooling2D(),
        layers.Dense(128,
                    kernel_regularizer=tf.keras.regularizers.l1_l2(l1=1e-5, l2=1e-4),
                    bias_regularizer=tf.keras.regularizers.l2(1e-4),
                    bias_initializer='zeros'),  
        layers.BatchNormalization(),
        layers.Activation('relu'),
        layers.Dropout(0.5),
        layers.Dense(64,
                    kernel_regularizer=tf.keras.regularizers.l1_l2(l1=1e-5, l2=1e-4),
                    bias_regularizer=tf.keras.regularizers.l2(1e-4)),
        layers.BatchNormalization(),
        layers.Activation('relu'),
        layers.Dropout(0.4),
        layers.Dense(num_classes, activation='softmax', bias_initializer='zeros')  
    ])
    optimizer = tf.keras.optimizers.Adam(learning_rate=LEARNING_RATE, clipnorm=1.0)
    model.compile(optimizer=optimizer, loss='categorical_crossentropy', metrics=['accuracy'])
    return model
def create_small_dataset_model(model_type="MobileNetV2"):
    print(f"\nCreating model optimized for small datasets using {model_type}...")
    if model_type == "MobileNetV2":
        base_model = MobileNetV2(input_shape=IMAGE_SIZE + (3,), include_top=False, weights='imagenet')
        model = create_standard_model(base_model, num_classes)
    elif model_type == "EfficientNetB0":
        base_model = EfficientNetB0(input_shape=IMAGE_SIZE + (3,), include_top=False, weights='imagenet')
        model = create_standard_model(base_model, num_classes)
    elif model_type == "ResNet50V2":
        base_model = ResNet50V2(input_shape=IMAGE_SIZE + (3,), include_top=False, weights='imagenet')
        base_model.trainable = False
        inputs = tf.keras.Input(shape=IMAGE_SIZE + (3,))
        x = base_model(inputs, training=False)
        x = layers.GlobalAveragePooling2D()(x)
        x = layers.Dense(128, 
                        kernel_regularizer=tf.keras.regularizers.l1_l2(l1=1e-5, l2=1e-4),
                        bias_regularizer=tf.keras.regularizers.l2(1e-4),
                        bias_initializer='zeros')(x)
        x = layers.BatchNormalization()(x)
        x = layers.Activation('relu')(x)
        x = layers.Dropout(0.5)(x)
        x = layers.Dense(64,
                        kernel_regularizer=tf.keras.regularizers.l1_l2(l1=1e-5, l2=1e-4),
                        bias_regularizer=tf.keras.regularizers.l2(1e-4))(x)
        x = layers.BatchNormalization()(x)
        x = layers.Activation('relu')(x)
        x = layers.Dropout(0.4)(x)
        outputs = layers.Dense(num_classes, activation='softmax', bias_initializer='zeros')(x)
        model = tf.keras.Model(inputs, outputs)
        model.compile(
            optimizer=tf.keras.optimizers.Adam(learning_rate=0.0002),
            loss='categorical_crossentropy',
            metrics=['accuracy']
        )
    else:
        raise ValueError(f"Unsupported model type: {model_type}")
    model.summary()
    return model, base_model
def train_model(model_type, model, base_model):
    print(f"\nStarting training for {model_type}...")
    callbacks = [
        tf.keras.callbacks.EarlyStopping(
            monitor='val_loss', patience=PATIENCE, restore_best_weights=True, verbose=1
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss', factor=0.5, patience=10, min_lr=1e-6, verbose=1
        ),
        tf.keras.callbacks.ModelCheckpoint(
            f'best_{model_type}_small_dataset_model.weights.h5',
            monitor='val_accuracy', save_best_only=True, save_weights_only=True, verbose=1
        ),
        tf.keras.callbacks.TensorBoard(log_dir=f'./logs/{model_type}', histogram_freq=1)
    ]
    print(f"\nPhase 1: Training top layers...")
    history = model.fit(
        train_dataset, epochs=EPOCHS1, validation_data=val_dataset,
        callbacks=callbacks, verbose=1, class_weight=class_weights
    )
    print(f"\nPhase 2: Fine-tuning base model...")
    base_model.trainable = True
    for layer in base_model.layers[:100]:
        layer.trainable = False
    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=LEARNING_RATE/10),
        loss='categorical_crossentropy', metrics=['accuracy']
    )
    history_fine = model.fit(
        train_dataset, epochs=EPOCHS2, validation_data=val_dataset,
        callbacks=callbacks, verbose=1, class_weight=class_weights
    )
    return history, history_fine
def evaluate_dataset(model_type, model, dataset_split='validation'):
    print(f"\nEvaluating {model_type} on {dataset_split} set...")
    eval_files, eval_labels = [], []
    for class_name in class_names:
        class_path = os.path.join(DATASET_DIR, class_name)
        if os.path.isdir(class_path):
            files = [
                os.path.join(class_path, f) for f in os.listdir(class_path) 
                if f.lower().endswith(('.png', '.jpg', '.jpeg'))
            ]
            if dataset_split == 'validation':
                indices = range(int(len(files) * 0.8), len(files))
            else:  
                indices = range(len(files))
            eval_files.extend([files[i] for i in indices])
            eval_labels.extend([class_names.index(class_name)] * len(list(indices)))
    y_true, y_pred = [], []
    class_correct = {cls: 0 for cls in class_names}
    class_total = {cls: 0 for cls in class_names}
    all_confidences = {cls: [] for cls in class_names}
    for file_path, true_label in zip(eval_files, eval_labels):
        img = tf.io.read_file(file_path)
        img = tf.image.decode_png(img, channels=3)
        img = tf.image.resize(img, IMAGE_SIZE)
        img = tf.cast(img, tf.float32) / 255.0
        img = tf.expand_dims(img, 0)  
        prediction = model.predict(img, verbose=0)[0]
        pred_label = np.argmax(prediction)
        confidence = prediction[pred_label]
        y_true.append(true_label)
        y_pred.append(pred_label)
        true_class = class_names[true_label]
        class_total[true_class] += 1
        all_confidences[true_class].append(confidence)
        if pred_label == true_label:
            class_correct[true_class] += 1
        print(f"File: {os.path.basename(file_path)}")
        print(f"  True: {true_class}, Predicted: {class_names[pred_label]} ({confidence:.2%})")
        print(f"  {'✓' if pred_label == true_label else '✗'}")
    print(f"\nPer-class accuracy ({dataset_split}) for {model_type}:")
    for cls in class_names:
        if class_total[cls] > 0:
            accuracy = class_correct[cls] / class_total[cls]
            avg_conf = np.mean(all_confidences[cls]) if all_confidences[cls] else 0
            print(f"  {cls}: {class_correct[cls]}/{class_total[cls]} = {accuracy:.2%} (avg conf: {avg_conf:.2%})")
            model_results[model_type]["class_accuracy"][cls] = accuracy
        else:
            print(f"  {cls}: No samples")
            model_results[model_type]["class_accuracy"][cls] = 0
    overall_accuracy = sum(class_correct.values()) / sum(class_total.values()) if sum(class_total.values()) > 0 else 0
    print(f"\nOverall accuracy: {overall_accuracy:.2%}")
    model_results[model_type]["val_accuracy"] = overall_accuracy
    if len(y_true) > 0:
        cm = confusion_matrix(y_true, y_pred)
        plt.figure(figsize=(10, 8))
        sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                    xticklabels=class_names, yticklabels=class_names)
        plt.title(f'Confusion Matrix - {model_type} ({dataset_split})')
        plt.xlabel('Predicted Label')
        plt.ylabel('True Label')
        plt.tight_layout()
        plt.savefig(f'confusion_matrix_{model_type}_{dataset_split}.png')
        plt.close()
        print(f"\nClassification Report ({dataset_split}):")
        print(classification_report(y_true, y_pred, target_names=class_names, zero_division=0))
    return overall_accuracy, y_true, y_pred
def test_on_custom_images(model_type, model, img_path_pattern, test_count, correct_class):
    print(f"\nTesting {model_type} on {test_count} images of {correct_class}...")
    correct, algorithm_predictions = 0, {cls: 0 for cls in class_names}
    all_confidences = []
    for test_num in range(1, test_count + 1):
        img_path = f"{img_path_pattern}{test_num}.png"
        if not os.path.exists(img_path):
            print(f"Warning: {img_path} not found, skipping.")
            continue
        img = tf.io.read_file(img_path)
        img = tf.image.decode_png(img, channels=3)
        img = tf.image.resize(img, IMAGE_SIZE)
        img = tf.cast(img, tf.float32) / 255.0
        img = tf.expand_dims(img, 0)  
        prediction = model.predict(img, verbose=0)[0]
        pred_label = np.argmax(prediction)
        pred_class = class_names[pred_label]
        confidence = prediction[pred_label]
        algorithm_predictions[pred_class] += 1
        all_confidences.append(confidence)
        if pred_class == correct_class:
            correct += 1
        print(f"\nTest {test_num}: {img_path}")
        print(f"  Prediction: {pred_class} ({confidence:.2%}), Expected: {correct_class}")
        print(f"  {'✓' if pred_class == correct_class else '✗'}")
        top_indices = np.argsort(prediction)[::-1][:3]
        print("  Top 3:")
        for idx in top_indices:
            print(f"    {class_names[idx]}: {prediction[idx]:.2%}")
    accuracy = correct / test_count if test_count > 0 else 0
    print(f"\nSummary - {correct_class}: {correct}/{test_count} = {accuracy:.2%}")
    if all_confidences:
        print(f"Average confidence: {np.mean(all_confidences):.2%}")
    model_results[model_type]["per_style_accuracy"][correct_class] = accuracy
    plt.figure(figsize=(12, 6))
    plt.bar(class_names, [algorithm_predictions[cls] for cls in class_names], color='skyblue')
    plt.xlabel('Algorithm Class')
    plt.ylabel('Count')
    plt.title(f'{model_type} Prediction Distribution for {correct_class}')
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.savefig(f'{model_type}_predictions_{correct_class}.png')
    plt.close()
    return correct, test_count
def check_initial_predictions(model_type, model):
    print(f"\nInitial predictions for {model_type}...")
    for class_name in class_names:
        class_path = os.path.join(DATASET_DIR, class_name)
        if os.path.isdir(class_path):
            files = [f for f in os.listdir(class_path) 
                    if f.lower().endswith(('.png', '.jpg', '.jpeg'))]
            if files:
                img_path = os.path.join(class_path, files[0])
                img = tf.io.read_file(img_path)
                img = tf.image.decode_png(img, channels=3)
                img = tf.image.resize(img, IMAGE_SIZE)
                img = tf.cast(img, tf.float32) / 255.0
                img = tf.expand_dims(img, 0)  
                preds = model.predict(img, verbose=0)[0]
                pred_class = class_names[np.argmax(preds)]
                pred_conf = np.max(preds)
                print(f"{class_name}: {pred_class} ({pred_conf*100:.2f}%)")
def plot_training_history(model_type, history, history_fine):
    if history is None or history_fine is None:
        print(f"Skipping training history plot for {model_type} (no history available)")
        return
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    axes[0].plot(history.history['accuracy'], label='Train Accuracy', 
                 color=MODEL_COLORS[model_type])
    axes[0].plot(history.history['val_accuracy'], label='Val Accuracy')
    if 'accuracy' in history_fine.history:
        last_epoch = len(history.history['accuracy'])
        fine_epochs = range(last_epoch + 1, last_epoch + len(history_fine.history['accuracy']) + 1)
        axes[0].plot(fine_epochs, history_fine.history['accuracy'], label='Fine-tune Train')
        axes[0].plot(fine_epochs, history_fine.history['val_accuracy'], label='Fine-tune Val')
    axes[0].set_title(f'{model_type} Accuracy')
    axes[0].set_ylabel('Accuracy')
    axes[0].set_xlabel('Epoch')
    axes[0].legend()
    axes[0].grid(True, linestyle='--', alpha=0.6)
    axes[1].plot(history.history['loss'], label='Train Loss')
    axes[1].plot(history.history['val_loss'], label='Val Loss')
    if 'loss' in history_fine.history:
        last_epoch = len(history.history['loss'])
        fine_epochs = range(last_epoch + 1, last_epoch + len(history_fine.history['loss']) + 1)
        axes[1].plot(fine_epochs, history_fine.history['loss'], label='Fine-tune Train')
        axes[1].plot(fine_epochs, history_fine.history['val_loss'], label='Fine-tune Val')
    axes[1].set_title(f'{model_type} Loss')
    axes[1].set_ylabel('Loss')
    axes[1].set_xlabel('Epoch')
    axes[1].legend()
    axes[1].grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(f'{model_type}_training_history.png')
    plt.close()
def plot_combined_training_history():
    fig, axes = plt.subplots(2, 2, figsize=(15, 8))
    for model_type in MODEL_TYPES:
        if model_results[model_type]["history"] is not None:
            axes[0, 0].plot(model_results[model_type]["history"].history['accuracy'], 
                           label=f'{model_type}')
    axes[0, 0].set_title('Training Accuracy')
    axes[0, 0].set_ylabel('Accuracy')
    axes[0, 0].set_xlabel('Epoch')
    axes[0, 0].legend()
    axes[0, 0].grid(True, linestyle='--', alpha=0.6)
    for model_type in MODEL_TYPES:
        if model_results[model_type]["history"] is not None:
            axes[0, 1].plot(model_results[model_type]["history"].history['val_accuracy'], 
                           label=f'{model_type}')
    axes[0, 1].set_title('Validation Accuracy')
    axes[0, 1].set_ylabel('Accuracy')
    axes[0, 1].set_xlabel('Epoch')
    axes[0, 1].legend()
    axes[0, 1].grid(True, linestyle='--', alpha=0.6)
    for model_type in MODEL_TYPES:
        if model_results[model_type]["history"] is not None:
            axes[1, 0].plot(model_results[model_type]["history"].history['loss'], 
                           label=f'{model_type}')
    axes[1, 0].set_title('Training Loss')
    axes[1, 0].set_ylabel('Loss')
    axes[1, 0].set_xlabel('Epoch')
    axes[1, 0].legend()
    axes[1, 0].grid(True, linestyle='--', alpha=0.6)
    for model_type in MODEL_TYPES:
        if model_results[model_type]["history"] is not None:
            axes[1, 1].plot(model_results[model_type]["history"].history['val_loss'], 
                           label=f'{model_type}')
    axes[1, 1].set_title('Validation Loss')
    axes[1, 1].set_ylabel('Loss')
    axes[1, 1].set_xlabel('Epoch')
    axes[1, 1].legend()
    axes[1, 1].grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig('combined_training_history.png')
    plt.close()
def plot_model_comparison():
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    overall_accs = [model_results[m]["val_accuracy"] for m in MODEL_TYPES]
    axes[0, 0].bar(MODEL_TYPES, overall_accs, 
                   color=[MODEL_COLORS[m] for m in MODEL_TYPES])
    axes[0, 0].set_ylabel('Accuracy')
    axes[0, 0].set_title('Overall Validation Accuracy')
    axes[0, 0].set_ylim(0, 1.0)
    for i, v in enumerate(overall_accs):
        axes[0, 0].text(i, v + 0.02, f'{v:.2%}', ha='center')
    x = np.arange(len(class_names))
    width = 0.35
    for i, model_type in enumerate(MODEL_TYPES):
        accs = [model_results[model_type]["class_accuracy"].get(cls, 0) for cls in class_names]
        axes[0, 1].bar(x + (i - 0.5) * width, accs, width, label=model_type,
                       color=MODEL_COLORS[model_type])
    axes[0, 1].set_ylabel('Accuracy')
    axes[0, 1].set_title('Per-Class Accuracy')
    axes[0, 1].set_xticks(x)
    axes[0, 1].set_xticklabels(class_names, rotation=45, ha='right')
    axes[0, 1].legend()
    axes[0, 1].set_ylim(0, 1.0)
    train_times = [model_results[m]["training_time"] for m in MODEL_TYPES]
    axes[1, 0].bar(MODEL_TYPES, train_times, 
                   color=[MODEL_COLORS[m] for m in MODEL_TYPES])
    axes[1, 0].set_ylabel('Time (seconds)')
    axes[1, 0].set_title('Training Time')
    for i, v in enumerate(train_times):
        axes[1, 0].text(i, v + max(train_times)*0.02, f'{v:.1f}s', ha='center')
    styles = list(set([key for m in MODEL_TYPES 
                      for key in model_results[m]["per_style_accuracy"].keys()]))
    if styles:
        x_style = np.arange(len(styles))
        for i, model_type in enumerate(MODEL_TYPES):
            accs = [model_results[model_type]["per_style_accuracy"].get(s, 0) for s in styles]
            axes[1, 1].bar(x_style + (i - 0.5) * width, accs, width, label=model_type,
                           color=MODEL_COLORS[model_type])
        axes[1, 1].set_ylabel('Accuracy')
        axes[1, 1].set_title('Per-Style Accuracy')
        axes[1, 1].set_xticks(x_style)
        axes[1, 1].set_xticklabels(styles, rotation=45, ha='right')
        axes[1, 1].legend()
        axes[1, 1].set_ylim(0, 1.0)
    plt.tight_layout()
    plt.savefig('model_comparison.png')
    plt.close()
def plot_confusion_matrices():
    all_y_true, all_y_pred = {}, {}
    for model_type in MODEL_TYPES:
        model = model_results[model_type]["model"]
        if model is None:
            continue
        val_files, val_labels = [], []
        for class_name in class_names:
            class_path = os.path.join(DATASET_DIR, class_name)
            if os.path.isdir(class_path):
                files = [
                    os.path.join(class_path, f) for f in os.listdir(class_path) 
                    if f.lower().endswith(('.png', '.jpg', '.jpeg'))
                ]
                val_indices = range(int(len(files) * 0.8), len(files))
                val_files.extend([files[i] for i in val_indices])
                val_labels.extend([class_names.index(class_name)] * len(list(val_indices)))
        y_true, y_pred = [], []
        for file_path, true_label in zip(val_files, val_labels):
            img = tf.io.read_file(file_path)
            img = tf.image.decode_png(img, channels=3)
            img = tf.image.resize(img, IMAGE_SIZE)
            img = tf.cast(img, tf.float32) / 255.0
            img = tf.expand_dims(img, 0)
            prediction = model.predict(img, verbose=0)[0]
            y_true.append(true_label)
            y_pred.append(np.argmax(prediction))
        all_y_true[model_type] = y_true
        all_y_pred[model_type] = y_pred
    fig, axes = plt.subplots(1, len(MODEL_TYPES), figsize=(len(MODEL_TYPES)*5, 5))
    if len(MODEL_TYPES) == 1:
        axes = [axes]
    for i, model_type in enumerate(MODEL_TYPES):
        if model_type in all_y_true and len(all_y_true[model_type]) > 0:
            cm = confusion_matrix(all_y_true[model_type], all_y_pred[model_type])
            sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', 
                        xticklabels=class_names, yticklabels=class_names, ax=axes[i])
            axes[i].set_title(f'{model_type}')
            axes[i].set_xlabel('Predicted')
            axes[i].set_ylabel('True')
    plt.tight_layout()
    plt.savefig('confusion_matrices_comparison.png')
    plt.close()
def save_model(model_type, model):
    print(f"\nSaving {model_type}...")
    try:
        model.save_weights(f'{model_type}_small_dataset_model.weights.h5')
        print(f"  Weights: {model_type}_small_dataset_model.weights.h5")
        model_json = model.to_json()
        with open(f"{model_type}_small_dataset_model.json", "w") as f:
            f.write(model_json)
        print(f"  Architecture: {model_type}_small_dataset_model.json")
    except Exception as e:
        print(f"Error saving {model_type}: {e}")
for model_type in MODEL_TYPES:
    try:
        print(f"\n{'='*60}")
        print(f"Training and evaluating {model_type}")
        print(f"{'='*60}")
        model, base_model = create_small_dataset_model(model_type)
        model_results[model_type]["model"] = model
        model_results[model_type]["base_model"] = base_model
        check_initial_predictions(model_type, model)
        start_time = time.time()
        history, history_fine = train_model(model_type, model, base_model)
        training_time = time.time() - start_time
        model_results[model_type]["training_time"] = training_time
        model_results[model_type]["history"] = history
        model_results[model_type]["history_fine"] = history_fine
        print(f"Training time: {training_time:.2f} seconds")
        plot_training_history(model_type, history, history_fine)
        evaluate_dataset(model_type, model, dataset_split='validation')
        evaluate_dataset(model_type, model, dataset_split='full')
        test_on_custom_images(model_type, model, 'dfs', COUNT_DFS_IMAGES, 'DFS')
        test_on_custom_images(model_type, model, 'fenwick', COUNT_FENWICK_IMAGES, 'fenwick')
        save_model(model_type, model)
    except Exception as e:
        print(f"Error processing {model_type}: {e}")
        traceback.print_exc()
print("\n" + "="*60)
print("MODEL COMPARISON RESULTS")
print("="*60)
print("\nOverall Validation Accuracy:")
for model_type in MODEL_TYPES:
    print(f"  {model_type}: {model_results[model_type]['val_accuracy']:.2%}")
print("\nPer-Class Accuracy:")
for cls in class_names:
    print(f"  {cls}:")
    for model_type in MODEL_TYPES:
        acc = model_results[model_type]["class_accuracy"].get(cls, 0)
        print(f"    {model_type}: {acc:.2%}")
styles = list(set([key for m in MODEL_TYPES 
                  for key in model_results[m]["per_style_accuracy"].keys()]))
if styles:
    print("\nPer-Style Accuracy:")
    for style in styles:
        print(f"  {style}:")
        for model_type in MODEL_TYPES:
            acc = model_results[model_type]["per_style_accuracy"].get(style, 0)
            print(f"    {model_type}: {acc:.2%}")
print("\nTraining Time:")
for model_type in MODEL_TYPES:
    print(f"  {model_type}: {model_results[model_type]['training_time']:.2f}s")
scores = {model_type: 0 for model_type in MODEL_TYPES}
val_accs = [model_results[m]["val_accuracy"] for m in MODEL_TYPES]
scores[MODEL_TYPES[np.argmax(val_accs)]] += 1
for cls in class_names:
    accs = [model_results[m]["class_accuracy"].get(cls, 0) for m in MODEL_TYPES]
    scores[MODEL_TYPES[np.argmax(accs)]] += 1
for style in styles:
    accs = [model_results[m]["per_style_accuracy"].get(style, 0) for m in MODEL_TYPES]
    scores[MODEL_TYPES[np.argmax(accs)]] += 1
train_times = [model_results[m]["training_time"] for m in MODEL_TYPES]
scores[MODEL_TYPES[np.argmin(train_times)]] += 1
best_model = max(scores.items(), key=lambda x: x[1])[0]
print(f"\nBest model: {best_model} (score: {scores[best_model]})")
for model_type, score in scores.items():
    print(f"  {model_type}: {score} points")
print("\n" + "="*60)
print("Creating visualizations...")
print("="*60)
try:
    plot_combined_training_history()
    plot_model_comparison()
    plot_confusion_matrices()
    print("\nAll visualizations created successfully!")
except Exception as e:
    print(f"Error creating visualizations: {e}")
    traceback.print_exc()