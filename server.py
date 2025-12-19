from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel

app = FastAPI()

# ==== MODEL DATA ====
class SensorData(BaseModel):
    temperature: float
    humidity: float

latest_data = {
    "temperature": None,
    "humidity": None
}

# ==== API ====
@app.post("/api/data")
def receive_data(data: SensorData):
    if data.temperature < -40 or data.temperature > 80:
        return {"error": "invalid temperature"}
    if data.humidity < 0 or data.humidity > 100:
        return {"error": "invalid humidity"}

    global latest_data
    latest_data = data.model_dump()
    return {"status": "ok"}


@app.get("/api/data")
def get_data():
    return latest_data

# ==== STATIC FILE ====
app.mount("/", StaticFiles(directory="dashboard", html=True), name="dashboard")
