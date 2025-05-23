import cv2
import numpy as np

def detect_color(frame, color_lower, color_upper):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, color_lower, color_upper)
    res = cv2.bitwise_and(frame, frame, mask=mask)
    return res

red_lower = np.array([0, 100, 100])
red_upper = np.array([10, 255, 255])

green_lower = np.array([40, 40, 40])
green_upper = np.array([70, 255, 255])

blue_lower = np.array([90, 100, 100])
blue_upper = np.array([130, 255, 255])

url = 'http://192.168.0.10/480x320.mjpeg'
cap = cv2.VideoCapture(url)
winName = 'IP_CAM'
cv2.namedWindow(winName, cv2.WINDOW_AUTOSIZE)

while True:
    cap.open(url)
    ret, frame = cap.read()
    if not ret:
        break
    
    red_detected = detect_color(frame, red_lower, red_upper)
    green_detected = detect_color(frame, green_lower, green_upper)
    blue_detected = detect_color(frame, blue_lower, blue_upper)
    
    cv2.imshow('Red Detected', red_detected)
    cv2.imshow('Green Detected', green_detected)
    cv2.imshow('Blue Detected', blue_detected)
    cv2.imshow(winName, frame)
    
    if cv2.waitKey(1) & 0xFF == 27:
        break

cv2.destroyAllWindows()
cap.release()
