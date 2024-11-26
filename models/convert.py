from ultralytics import YOLO

model = YOLO("0907_epoch15_model.pt")
model.export(format="onnx", imgsz=[640, 640], opset=12)
