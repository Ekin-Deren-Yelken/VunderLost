import csv, math, os, random

# =============================================================
# Simulated Doppler Signal Generator for BPM Display Testing
# =============================================================
#
# Assumptions:
# - Each heartbeat is represented by a sine burst with a cos² amplitude envelope.
# - The burst frequency is calculated using the Doppler equation:
#     f_d = (2 * f0 * v * cos(θ)) / c
# - Amplitude is normalized to ±1.0.
# - Sampling rate is fixed at 1000 Hz for simplicity.
# - Burst shape does not simulate beam attenuation or scattering.
# =============================================================


# CONFIG
DURATION_SEC = 10               # Duration of the signal in seconds
SAMPLE_RATE_HZ = 1000           # Sampling rate (1 ms resolution)
TARGET_BPM = 60                 # Target heart rate
BPM_VARIANCE = 2                # Small timing variation
BURST_DURATION_MS = 30          # Duration of each burst

# DOPPLER
BASE_FREQUENCY_HZ = 2_000_000   # Transmit frequency (e.g., 2 MHz)
VELOCITY_M_S = 0.5              # Flow speed (m/s)
ANGLE_DEGREES = 60              # Beam-to-flow angle
C_SOUND = 1540                  # Speed of sound in soft tissue (m/s)

# ARTIFACTS
DROPOUT_MODE = True
NOISE_MODE = True
JITTER_MODE = True
OUTPUT_FILE = "NORMAL.csv"

# Calculates doppler shift
# Returns: int: doppler frequency f_d
def calculate_doppler_shift(f0, velocity, angle_deg, c=C_SOUND):
    theta_rad = math.radians(angle_deg)
    return (2 * f0 * velocity * math.cos(theta_rad)) / c


# Generates a list of heartbeat timestamps (in ms) over a specified duration
# Returns: list[int]: A list of timestamps in milliseconds indicating beat locations
def generate_heartbeat_times(bpm, duration_sec, variance=0, dropout=False, jitter=False):
    interval_ms = 60000 / bpm
    timestamps = []
    t = 0
    
    while t < duration_sec * 1000:
        jitter_offset = random.uniform(-30, 30) if jitter else 0                                   # Apply jitter
        
        actual_interval = interval_ms + jitter_offset + random.uniform(-variance, variance)        # Apply variation to heart rate
        
        if dropout and random.random() < 0.05:                                                     # Drop beat randomly
            t += actual_interval
            continue
        
        timestamps.append(int(t))
        t += actual_interval
    return timestamps

# Generates a Doppler-like analog signal with sine bursts at specified heartbeat time
# Returns: list[float]: Simulated analog signal sampled at the specified rate
def generate_signal(duration_sec, sample_rate_hz, beat_times_ms, burst_freq_hz, burst_duration_ms, noise=False):
    total_samples = duration_sec * sample_rate_hz
    signal = [0.0] * total_samples
    burst_samples = int(burst_duration_ms * sample_rate_hz / 1000)

    # Precompute cos^2 envelope
    envelope = [math.cos(math.pi * n / (burst_samples - 1))**2 for n in range(burst_samples)]

    for beat_time in beat_times_ms:
        burst_start = int(beat_time)
        for n in range(burst_samples):
            i = burst_start + n
            if i < len(signal):
                signal[i] += envelope[n] * math.sin(2 * math.pi * doppler_freq_hz * n / sample_rate_hz)

    # Add optional noise
    if noise:
        for i in range(len(signal)):
            if random.random() < 0.01:
                signal[i] += random.uniform(-0.3, 0.3)

    return signal

# Writes the simulated signal to a CSV file with timestamps
def write_csv(filename, signal, sample_rate_hz):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["timestamp_ms", "amplitude"])
        
        for i, val in enumerate(signal):
            timestamp = int(i * (1000 / sample_rate_hz))
            writer.writerow([timestamp, f"{val:.4f}"])

if __name__ == "__main__":
    print("[INFO] Generating simulated Doppler-like signal...")
    
    # Compute physics-based Doppler shift
    doppler_freq = calculate_doppler_shift(BASE_FREQUENCY_HZ, VELOCITY_M_S, ANGLE_DEGREES)
    print(f"[INFO] Doppler burst frequency: {doppler_freq:.2f} Hz")

    # Generate heartbeat timing
    beat_times = generate_heartbeat_times(
        bpm=TARGET_BPM,
        duration_sec=DURATION_SEC,
        variance=BPM_VARIANCE,
        dropout=DROPOUT_MODE,
        jitter=JITTER_MODE
    )

    # Generate analog signal with bursts
    signal = generate_signal(
        duration_sec=DURATION_SEC,
        sample_rate_hz=SAMPLE_RATE_HZ,
        beat_times_ms=beat_times,
        burst_duration_ms=BURST_DURATION_MS,
        doppler_freq_hz=doppler_freq,
        noise=NOISE_MODE
    )

    # Write to CSV
    write_csv(OUTPUT_FILE, signal, SAMPLE_RATE_HZ)
    print(f"[INFO] File written: {OUTPUT_FILE}")
