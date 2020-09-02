import requests 
r = requests.get("http://localhost:8080/?reserva=1")
print(r.text)