import cv2
import mediapipe as mp
import serial
import time

# 🔴 CHANGE COM PORT
ser = serial.Serial("COM12", 115200, timeout=1)
time.sleep(2)

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1)
mp_draw = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)
last_cmd = ""

def count_fingers(hand_landmarks):
    fingers = 0
    tips = [8, 12, 16, 20]
    for tip in tips:
        if hand_landmarks.landmark[tip].y < hand_landmarks.landmark[tip-2].y:
            fingers += 1
    return fingers

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb)

    cmd = "NONE"

    if result.multi_hand_landmarks:
        for hand in result.multi_hand_landmarks:
            mp_draw.draw_landmarks(frame, hand, mp_hands.HAND_CONNECTIONS)
            fingers = count_fingers(hand)

            if fingers == 0:
                cmd = "OFF"
            elif fingers == 5:
                cmd = "ON"
            elif fingers == 1:
                cmd = "BLINK"

    if cmd != last_cmd and cmd != "NONE":
        ser.write((cmd + "\n").encode())
        print("Sent:", cmd)
        last_cmd = cmd

    cv2.putText(frame, f"CMD: {cmd}", (30, 50),
                cv2.FONT_HERSHEY_SIMPLEX, 1,
                (0, 255, 0), 2)

    cv2.imshow("Gesture LED Control", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
ser.close()
