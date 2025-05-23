import cv2
import numpy as np

def detect_color(frame, color_lower, color_upper):
    # Convertir la imagen de BGR a HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Definir un rango de colores a detectar en HSV
    mask = cv2.inRange(hsv, color_lower, color_upper)
    
    # Aplicar la máscara al frame original
    res = cv2.bitwise_and(frame, frame, mask=mask)
    
    return res

# Definir los rangos de color para rojo, verde y azul en formato HSV
red_lower = np.array([0, 100, 100])
red_upper = np.array([10, 255, 255])

green_lower = np.array([40, 40, 40])
green_upper = np.array([70, 255, 255])

blue_lower = np.array([90, 100, 100])
blue_upper = np.array([130, 255, 255])

url = 'http://192.168.114.140:81/stream'
cap = cv2.VideoCapture(url)
winName = 'IP_CAM'
cv2.namedWindow(winName, cv2.WINDOW_AUTOSIZE)

while True:
    cap.open(url)  # Abre la URL para capturar el frame
    ret, frame = cap.read()
    if not ret:
        break
    
    # Detectar los colores en el frame actual
    red_detected = detect_color(frame, red_lower, red_upper)
    green_detected = detect_color(frame, green_lower, green_upper)
    blue_detected = detect_color(frame, blue_lower, blue_upper)
    
    # Combinar las imágenes detectadas
    combined = cv2.addWeighted(red_detected, 1/3, green_detected, 1/3, 0)
    combined = cv2.addWeighted(blue_detected, 1/3, combined, 1, 0)
    
    # Mostrar los resultados de detección de color en una sola ventana
    cv2.imshow(winName, combined)
    
    # Salir si se presiona la tecla 'Esc'
    if cv2.waitKey(1) & 0xFF == 27:
        break

cv2.destroyAllWindows()
cap.release()
