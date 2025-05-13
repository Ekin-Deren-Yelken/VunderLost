from transformers import pipeline
import torch
import sys
import time

# Define model and revision explicitly to avoid delay
model_name = "distilbert/distilbert-base-uncased-finetuned-sst-2-english"
revision = "714eb0f"  # This is the revision mentioned in the warning

# Safely fallback to CPU
device = 0 if torch.cuda.is_available() else -1
classifier = pipeline("sentiment-analysis", model=model_name, revision=revision, device=device)

# Get input from argv
text = " ".join(sys.argv[1:])

# Run sentiment analysis
start = time.time()
result = classifier(text)[0]
elapsed = time.time() - start

label = result['label']
score = result['score']

# Map to game-friendly categories
if label == "POSITIVE" and score >= 0.7:
    print("friendly")
elif label == "NEGATIVE" and score >= 0.7:
    print("hostile")
else:
    print("neutral")

# debug:
# print(f"[DEBUG] Label: {label} | Score: {score:.2f} | Time: {elapsed:.2f}s")
