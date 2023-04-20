const express = require("express")
const http = require("http")
const net = require("net")

const WebSocket = require("ws")

const { StartClientHandler } = require("./src/clients.js")
const { ConnectArduino } = require("./src/arduino.js")
const { Writable, Readable } = require("stream")

const app = express()
const http_server = http.createServer(app)
const tcp_server = net.createServer()
const ws = new WebSocket.Server({ server: http_server })

/** @type {Writable} */
const arduinoIn = new Writable()

/** @type {Writable} */
const arduinoOut = new Writable()

tcp_server.on("connection", async (socket) => {
	console.log("Client connected")

	//wait until data recieved
	const data = await new Promise((resolve) => {
		socket.once("data", (data) => resolve(data))
	})

	//if first byte is '!'
	if (data[0] == 33) {
		ConnectArduino(socket, arduinoIn, arduinoOut)
		return
	}

	//resume http
	http_server.emit("connection", socket)
	socket.emit("data", data)
})

//start the websocket handler
StartClientHandler(ws, arduinoIn, arduinoOut)

//serve all files in public folder
app.use(express.static("public"))

//listern on port 80
tcp_server.listen(80, () => {
	console.log("Server is listening on port 80")
})
