from transformers import pipeline
import torch
import sys
import time

# Safely fallback to CPU
device = 0 if torch.cuda.is_available() else -1
classifier = pipeline("sentiment-analysis", device=device)

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

# Optional: print for debug
# print(f"[DEBUG] Raw Label: {label} ({score:.2f}) | Time: {elapsed:.3f}s")
