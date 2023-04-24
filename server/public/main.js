let tempratureElement = document.getElementById("temprature")
let humidityElement = document.getElementById("humidity")
let clicksElement = document.getElementById("clicks")
const childeren = document.getElementById("colors").children

console.log(tempratureElement, humidityElement)

/** @type {WebSocket} */
let ws

function connect() {
	//websockets explained in the document
	ws = new WebSocket(`ws://${window.location.host}:80`)

	ws.onmessage = (e) => {
		/** @type {string} data */
		const data = e.data

		if (data.startsWith("set_clicks:")) {
			clicksElement.innerText = data.replace("set_clicks:", "")
		}

		if (data.startsWith("temprature:")) {
			tempratureElement.innerText = data.replace("temprature:", "") + "°C"
		}

		if (data.startsWith("humidity:")) {
			humidityElement.innerText = data.replace("humidity:", "") + "%"
		}

		console.log("[WS] [SERVER] -> ", e.data)
	}

	ws.onopen = () => {
		console.log("[WS] connected to server")
	}

	ws.onclose = () => {
		console.log("[WS] lost connection")

		setTimeout(function () {
			console.log("[WS] reconnecting...")
			connect()
		}, 1000) // retry after 1 second
	}
}

connect()

for (let i = 0; i < childeren.length; i++) {
	const child = childeren.item(i)

	if (i < 7) {
		child.addEventListener("click", () => ws.send(`status_led:${i}`))
		continue
	}

	child.addEventListener("click", () =>
		ws.send(`status_led:${child.textContent}`)
	)
}
