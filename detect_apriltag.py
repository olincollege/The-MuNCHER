import cv2
import apriltag 
from apriltag_import import Detector
import numpy as np
from calibrate import calibrate

with open('camera_calibration_params.npy', 'rb') as f:
    ret = np.load(f)
    camera_matrix = np.load(f)
    distortion_coefs = np.load(f)

# [row, column]
camera_params = [camera_matrix[0, 0], camera_matrix[1, 1], camera_matrix[0, 2], camera_matrix[1, 2]]
tag_size = 0.165 # this is the apriltag size

vid = cv2.VideoCapture(0)
vid.set(cv2.CAP_PROP_FPS, 30) # adjust frame rate for lower latency

options = apriltag.DetectorOptions(families="tag36h11")
detector = apriltag.Detector(options)

r_tag_id = []
results_data = []

while (True):
    ret, frame = vid.read()
    if not ret:
        print("Failed to capture image")
        break
    
    frame = cv2.flip(frame, -1) # flip camera to adjust for problems with how it's mounted on rover
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) 

    results = detector.detect(gray)
    results_data.append(results)

    # load the numpy file with correct parameters so we can access camera parameters
    for r in results:
        detector = Detector() # this worked? why?
        pose, init_error, final_error = detector.detection_pose(r, camera_params, tag_size)
        (ptA, ptB, ptC, ptD) = r.corners
        ptB = (int(ptB[0]), int(ptB[1]))
        ptC = (int(ptC[0]), int(ptC[1]))
        ptD = (int(ptD[0]), int(ptD[1]))
        ptA = (int(ptA[0]), int(ptA[1]))

        cv2.line(frame, ptA, ptB, (0, 255, 0), 2)
        cv2.line(frame, ptB, ptC, (0, 255, 0), 2)
        cv2.line(frame, ptC, ptD, (0, 255, 0), 2)
        cv2.line(frame, ptD, ptA, (0, 255, 0), 2)

        # changed this from (pose[0][:, 3]) since I just want the translation vector from the camera to tag and that's the 4th vector
        distance = np.linalg.norm(pose[:, 3]) # pose returns a translation and rotation vector to describe orientation
        rotation_matrix = pose[0:3, 0:3]
        translation_vector = pose[0:3, 3]
        # this method converts a rotation vector into a rotation matrix. It expects a 3x1 or 1x3 array
        rvec, _ = cv2.Rodrigues(rotation_matrix)
        sy = np.sqrt(rotation_matrix[0,0] * rotation_matrix[0,0] +  rotation_matrix[1,0] * rotation_matrix[1,0])
        singular = sy < 1e-6
        if not singular: # standard case
            x = np.arctan2(rotation_matrix[2,1], rotation_matrix[2,2])
            y = np.arctan2(-rotation_matrix[2,0], sy)
            z = np.arctan2(rotation_matrix[1,0], rotation_matrix[0,0])
        else: # gimbal lock case
            x = np.arctan2(-rotation_matrix[1,2], rotation_matrix[1,1])
            y = np.arctan2(-rotation_matrix[2,0], sy)
            z = 0

        # Convert radians to degrees
        angles = np.degrees([x, y, z])
        angles = np.degrees(angles)
        results_data.append({
            'tag_id' : r.tag_id,
            'pose' : pose, 
            'distance' : distance, 
            'angle' : angles,
        })
    
    if r_tag_id:
        max_tag = max(r_tag_id)
    # process results here - find highest value tag

    cv2.imshow('Webcam', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

vid.release()
cv2.destroyAllWindows

print(r_tag_id)
print(results_data)
