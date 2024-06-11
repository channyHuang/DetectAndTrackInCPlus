from ultralytics import YOLO
model = YOLO("yolov8n.pt")
# model.export(format="onnx", opset=12, simplify=True, dynamic=False, imgsz=1920)

result = model.track(source='./1945.MP4', show = True, save = False)