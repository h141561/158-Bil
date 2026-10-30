import gradio as gr
import joblib
import pandas as pd

model = joblib.load("bil_forest.joblib")

def predict(km, aar, vekt, hk, kwh, gir, motor, merke):
    row = pd.DataFrame([{
        "kmStand": km, "modellAar": aar, "vekt": vekt, "hk": hk, "kwh": kwh,
        "gir": gir, "motor": motor, "Merke": merke,
    }])
    price = model.predict(row)[0]
    return f"{price:,.0f} kr"

demo = gr.Interface(
    fn=predict,
    inputs=[
        gr.Number(label="Kilometerstand", value=100000),
        gr.Number(label="Modellår", value=2015),
        gr.Number(label="Vekt (kg)", value=1500),
        gr.Number(label="Hestekrefter", value=150),
        gr.Number(label="Batterikapasitet (kWh)", value=0),
        gr.Dropdown(["automat", "manuell"], label="Girkasse", value="automat"),
        gr.Dropdown(["bensin", "diesel", "el"], label="Drivstoff", value="bensin"),
        gr.Textbox(label="Merke", value="Toyota"),
    ],
    outputs=gr.Textbox(label="Estimert pris"),
    title="Bilprisestimat",
)

demo.launch()
