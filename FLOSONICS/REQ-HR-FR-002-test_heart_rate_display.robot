*** Settings ***
Library           SeleniumLibrary      # Automate browser interaction, field access, and screenshots
Library           OperatingSystem      # File Operations
Library           Process              # Run Python script
Suite Setup       Setup Test App       # Open Browser before tests
Suite Teardown    Close Browser        # Close Browser

*** Variables ***
${URL}                http://localhost:8000/test_ui    # Web application for testing the UI
${SCREENSHOT_DIR}     screenshots                      # Dictionary to store Screenshots.
${HEART_RATE_FILE}    bpm_log.txt                      # File to store heart rate values
${PYTHON_SCRIPT}      TC-01.py                         # Python script for analyzing outputs
${SAMPLES}            10                               # Total Number of Samples. 
${BROWSER}            Chrome                           # Browser to Connect to Application.

*** Test Cases ***
REQ-HR-FR-002-TC-01_Verify_BPM_Calculation_and_Display
    [Documentation]    Capture 10 heart rate samples and screenshots every second for 10 seconds.
    [Tags]    heart_rate    ui    functional    class_B    REQ-HR-FR-002
    [Timeout]    60 seconds        # Timeout after specified time

    Create Dictionary    ${SCREENSHOT_DIR}
    # Go to the application and wait for webpage to load
    Go To    ${URL}
    Sleep    5 seconds       
    
    # Click the "Start" or "Begin Data" button to trigger live updates
    Click Button    xpath=//button[@id='start']
    Sleep    5 seconds

    # Once data stream begins, obtain data approximately ever second
    FOR  ${i}  IN RANGE  ${SAMPLES}:
        # Extract Heart Rate value in BPM from correct field and assign it to a variable
        ${HEART_RATE_DISPLAY}=    Run Keyword And Ignore Error    Get Text    xpath=//div[@id='heart-rate']

        Log To Console    ${HEART_RATE_DISPLAY}
        Append To File    bpm_log.txt    ${i}: ${HEART_RATE_DISPLAY}

        # Obtain screenshot for comparison
        Capture Page Screenshot    ${SCREENSHOT_DIR}/screenshot_${i}.png
        Sleep    1 second
    END

    # Run Validation
    Validate Heart Rate Output

REQ-HR-FR-002-TC-02_
    [Documentation]    Capture 10 heart rate samples and screenshots every second for 10 seconds.
    [Tags]    heart_rate    ui    functional    class_B    REQ-HR-FR-002
    [Timeout]    60 seconds        # Timeout after specified time
    Sleep    1 second

*** Keywords ***
Setup Test App
    Open Browser    ${URL}    ${BROWSER}    # Access URL in browser
    Maximize Browser Window                 # Ensure consistent screenshots

Validate Heart Rate Output
    ${result}=    Run Process    python    ${PYTHON_SCRIPT}    shell=True
    Should Contain    ${result.stdout}    PASS