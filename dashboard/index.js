/** @format */

async function loadData() {
  const res = await fetch("/api/data");
  const data = await res.json();

  if (data.temperature !== null) {
    document.getElementById("temp").innerText = data.temperature;
    document.getElementById("hum").innerText = data.humidity;
  }
}

setInterval(loadData, 2000);
