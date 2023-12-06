# write original functions to calibrate
# perhaps write a function to recalibrate if something changes (how will we know and what are going to do about it?)
import cv2
import numpy as np
import os


def retrieve_images(directory):
    images = []
    for image in os.listdir(directory):
        i = os.path.join(directory, image)
        images.append(i)
    return images

def calibrate(images):
    # simple loop that turns all images (jpg) into a cv2 object array
    cv2objects = []
    for obj in images:
        save = cv2.imread(obj)
        cv2objects.append(save)
    
    # calibrate the camera with 3D points and 2D from chessboard list
    # Number of internal corners
    width = 8  # Internal corners along the width
    height = 6  # Internal corners along the height

    # Assuming each square is 30mm x 30mm
    square_size = 30
    object_points = np.zeros((width * height, 3), np.float32)
    object_points[:, :2] = np.mgrid[0:width, 0:height].T.reshape(-1, 2) * square_size
    
    imgpoints = []
    objpoints = []
    for obj in cv2objects:
        gray = cv2.cvtColor(obj, cv2.COLOR_BGR2GRAY)
        flag, corners = cv2.findChessboardCorners(gray, (8, 6), None)
        if flag == True:
            objpoints.append(object_points) # append the *same* obj points to the list at same index as imgpoints
            imgpoints.append(np.float32(corners)) 

    ret, camera_matrix, distortion_coeffs, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
    return ret, camera_matrix, distortion_coeffs # these are ... 

def save_params(ret, camera_matrix, distortion_coefs):
    with open('camera_calibration_params.npy', 'wb') as f:
        np.save(f, ret)
        np.save(f, camera_matrix)
        np.save(f, distortion_coefs)

# Function calling
images_for_calibration = retrieve_images('/Users/zaynpatel/Downloads/PIEPhotos')
ret, cam_matrix, distort_coefs = calibrate(images_for_calibration)
saving_params = save_params(ret, cam_matrix, distort_coefs)
#print(f'this is ret {ret}')
#print(f'this is the camera matrix {cam_matrix}')
#print(f'this is the distort coefficient {distort_coefs}')