import ctypes
import multiprocessing
import numpy as np
import threading


# 目标检测返回数据结构
class stDetectResult(ctypes.Structure):
    _fields_ = [('pFrame', ctypes.c_void_p), 
                ('nDetectNum', ctypes.c_int), 
                ('nWidth', ctypes.c_int), 
                ('nHeight', ctypes.c_int), 
                ('pClasses', ctypes.POINTER(ctypes.c_int)), 
                ('pBoxes', ctypes.POINTER(ctypes.c_int)), 
                ('pProb', ctypes.POINTER(ctypes.c_float))]
    
class DetectTrackInC():
    _instance_lock = threading.Lock()

    def __new__(cls, *args, **kwargs):
        if not hasattr(DetectTrackInC, "_instance"):
            with DetectTrackInC._instance_lock:
                if not hasattr(DetectTrackInC, "_instance"):
                    DetectTrackInC._instance = object.__new__(cls)
        return DetectTrackInC._instance
    
    def __init__(self):
        self.lib = None
        self.libpath = '../lib/build/libHGStreamLib.a'
        self.alive = multiprocessing.Manager().Value('b', False)
    
    def __init__(self) -> None:
        self.lib.detect.argtype = (ctypes.POINTER(ctypes.c_char_p), ctypes.c_int, ctypes.c_int)
        self.lib.detect.restype = ctypes.POINTER(stDetectResult)

    def detect(self, frame):
        if self.lib is None:
            return None, None, None, None
        
        dataPtr = frame.ctypes.data_as(ctypes.POINTER(ctypes.c_char_p))
        detectResult = self.lib.detect(dataPtr, frame.shape[1], frame.shape[0])
        
        num = detectResult.contents.nDetectNum
        classes = []
        boxes = []
        prob = []
        if num > 0:
            # 目标类型
            classesPtr = ctypes.cast(detectResult.contents.pClasses, ctypes.POINTER(ctypes.c_int * num))
            classes = np.frombuffer(classesPtr.contents, dtype=np.int32, count = num)
            # 目标框
            boxesPtr = ctypes.cast(detectResult.contents.pBoxes, ctypes.POINTER(ctypes.c_int * num * 4))
            boxes_raw = np.frombuffer(boxesPtr.contents, dtype=np.int32, count = num * 4)
            boxes = []
            box = []
            for i, b in enumerate(boxes_raw):
                box.append(b)
                if i % 4 == 3:
                    boxes.append(box)
                    box = []
            # 目标置信度
            probPtr = ctypes.cast(detectResult.contents.pProb, ctypes.POINTER(ctypes.c_float * num))
            prob = np.frombuffer(probPtr.contents, dtype = np.float32, count = num)
        # 带框图像
        nWidth = detectResult.contents.nWidth
        nHeight = detectResult.contents.nHeight
        byteCount = nWidth * nHeight * 3
        imagePtr = ctypes.cast(detectResult.contents.pFrame, ctypes.POINTER(ctypes.c_uint8 * byteCount))
        picture = np.frombuffer(imagePtr.contents, dtype=np.ubyte, count=byteCount)
        picture = np.reshape(picture, (nHeight, nWidth, 3))
        return picture, classes, boxes, prob
    
detectTrackInC = DetectTrackInC()

if __name__ == '__main__':
    import cv2
    frame = cv2.imread('')
    picture, classes, boxes, prob = detectTrackInC.detect(frame)
    cv2.imshow('res', picture)
    cv2.waitKey(10000)