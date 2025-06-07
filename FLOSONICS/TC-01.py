import cv2, os, datetime, sys, requests, base64
import numpy as np
from skimage.metrics import structural_similarity as ssim

JIRA_AUTH = (os.environ["JIRA_USER"], os.environ["JIRA_TOKEN"]) #use environmental variables for added security

# --- Configuration ---
EXPECTED_BPM = 60
TOLERANCE = 2

SIMILARITY_TOLERANCE = 0.98

LOG_FILE = "tc01_log.txt"
BPM_FILE = "bpm_log.txt"
SCREENSHOT_DIR = "screenshots"
REFERENCE_DIR = "reference_images"
ROI = (0, 2500, 0, 500) # region of interest (heart rate monitor)

# --- Logging Utility ---
def log(message):
    
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    with open(LOG_FILE, "a") as f:
        f.write(f"[{timestamp}] {message}\n")
    
# --- BPM Field Check ---
def check_numeric_values():
    
    if not os.path.exists(BPM_FILE):
        log("BPM File not Found")
        return False
    
    with open(BPM_FILE, "r") as f:
        bpm_values = [int(line.strip()) for line in f.readlines()]
    
    total_bpm = 0
    
    for i, ACTUAL_BPM in enumerate(bpm_values):
        if not (EXPECTED_BPM - TOLERANCE <= ACTUAL_BPM <= EXPECTED_BPM + TOLERANCE):
            log(f"Sample {i+1}: BPM out of range: {ACTUAL_BPM}, Expected {EXPECTED_BPM - TOLERANCE} BPM - {EXPECTED_BPM + TOLERANCE} BPM")
            return False
        log(f"Sample {i+1}: BPM = {ACTUAL_BPM}")
        total_bpm += ACTUAL_BPM
    
    # Calculate average BPM and round to nearest integer
    avg_bpm = round(total_bpm/len(bpm_values))
    
    if not (EXPECTED_BPM - TOLERANCE <= avg_bpm <= EXPECTED_BPM + TOLERANCE):
        log(f"Average BPM out of range: {avg_bpm}, Expected {EXPECTED_BPM - TOLERANCE} BPM - {EXPECTED_BPM + TOLERANCE} BPM")
        return False
        
    return True

# --- Image Region Match ---
def match_bpm_display(full_image_path, bpm_template_path, roi=ROI, threshold=0.98):
    try:
        full_img = cv2.imread(full_image_path)
        template_img = cv2.imread(bpm_template_path)

        if full_img is None or template_img is None:
            log(f"Could not load one or both images: {full_image_path}, {bpm_template_path}")
            return False

        # Crop screenshot to only contain heart rate display
        y1, y2, x1, x2 = roi
        cropped_img = full_img[y1:y2, x1:x2]

        # Compare reference image and cropped screenshot
        gray_cropped = cv2.cvtColor(cropped_img, cv2.COLOR_BGR2GRAY)
        gray_template = cv2.cvtColor(template_img, cv2.COLOR_BGR2GRAY)

        result = cv2.matchTemplate(gray_cropped, gray_template, cv2.TM_CCOEFF_NORMED)
        _, max_val, _, _ = cv2.minMaxLoc(result)

        log(f"Match score: {max_val:.3f}")
        return max_val >= threshold

    except Exception as e:
        log(f"Image comparison failed: {e}")
        return False

# --- Image Comparison ---
def check_images():
    with open(BPM_FILE, "r") as f:
        bpm_values = [int(line.strip()) for line in f.readlines()]
        
    passed = True
        
    for i in range(len(bpm_values)):
        log(f"===SAMPLE {i+1}===")
        
        bpm = bpm_values[i]
        screenshot = f"{SCREENSHOT_DIR}/screenshot_{i}.png"
        
        # Standardized file naming structure
        reference = f"{REFERENCE_DIR}/IOS_BPM_CHECK_HR_{bpm}.png"
        
        if not os.path.exists(reference):
            log(f"  Reference image missing: {reference} for {bpm} BPM, out of range")
            passed = False
            continue
        
        match = match_bpm_display(screenshot, reference, roi=ROI)

        if match:
            log(f"Sample {i+1}: BPM display match found in screenshot.")
        else:
            log(f"Sample {i+1}: BPM display not found or mismatch.")
            passed = False

    return passed

# --- Open JIRA issue ---
def create_jira_issue(summary, description, screenshot_paths=None):
    JIRA_URL = "https://your-domain.atlassian.net/rest/api/2/issue"
    PROJECT_KEY = "PROJ"
    ISSUE_TYPE = "Bug"

    headers = {
        "Content-Type": "application/json"
    }

    payload = {
        "fields": {
            "project": {"key": PROJECT_KEY},
            "summary": summary,
            "description": description,
            "issuetype": {"name": ISSUE_TYPE}
        }
    }

    response = requests.post(JIRA_URL, json=payload, auth=JIRA_AUTH, headers=headers)

    if response.status_code == 201:
        issue_key = response.json()["key"]
        print(f"JIRA issue created: {issue_key}")
        if screenshot_paths:
            attach_files_to_jira(issue_key, screenshot_paths)
        return issue_key
    else:
        print(f"Failed to create JIRA issue: {response.text}")
        return None

# --- Attach Files to JIRA issue ---
def attach_files_to_jira(issue_key, file_paths):
    JIRA_BASE_URL = "https://your-domain.atlassian.net"
    ATTACH_URL = f"{JIRA_BASE_URL}/rest/api/2/issue/{issue_key}/attachments"

    headers = {
        "X-Atlassian-Token": "no-check"
    }

    for path in file_paths:
        if os.path.exists(path):
            with open(path, "rb") as f:
                files = {
                    "file": (os.path.basename(path), f, "image/png")
                }
                response = requests.post(ATTACH_URL, headers=headers, files=files, auth=JIRA_AUTH)

                if response.status_code == 200:
                    log(f"Attached {path} to JIRA issue {issue_key}")
                else:
                    log(f"Failed to attach {path}: {response.status_code} - {response.text}")

if __name__ == "__main__":
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)
        
    log("=== Initializing Test Case: TC-01 ===")
    
    numeric_pass = check_numeric_values()
    image_pass = check_images()
    
    if numeric_pass and image_pass:
        log("[TEST RESULT]      PASS      ")
        print("PASS") # Required for Robot Framework validation
            
        sys.exit(0)
    elif not numeric_pass: 
        log("[TEST RESULT]      FAIL      ")
        log("      Calculated BPM values out of expected range")
        
        print("FAIL")   # Robot Framework Validation
        
        # Create JIRA issue
        with open(LOG_FILE, "r") as f:
            log_content = f.read()

            create_jira_issue(
                summary="TC-01 Failure: BPM display error. Calculated BPM out of range.",
                description=f"Log:\n{log_content}"
            )
            
        sys.exit(1)
    else:
        log("[TEST RESULT]      FAIL      ")
        log("      UI Screenshots do not match reference images")
        print("FAIL")

        # Gather screenshot paths
        screenshot_paths = [os.path.join(SCREENSHOT_DIR, f) for f in os.listdir(SCREENSHOT_DIR) if f.endswith(".png")]

        # Create JIRA issue
        with open(LOG_FILE, "r") as f:
            log_content = f.read()
            create_jira_issue(
                summary="TC-01 Failure: BPM display error. Image comparison failed.",
                description=f"Log:\n{log_content}",
                screenshot_paths=screenshot_paths
            )
            
        sys.exit(1)
