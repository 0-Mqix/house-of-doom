const express = require("express")
const http = require("http")
const net = require("net")

const WebSocket = require("ws")
const { Writable } = require("stream")

const { HandleArduinoConnection } = require("./src/arduino.js")
const { StartClientHandler } = require("./src/clients.js")

const app = express()
const http_server = http.createServer(app)
const tcp_server = net.createServer()
const ws = new WebSocket.Server({ server: http_server })

//a Writable is an object that is a stream where you can
//attach
/** @type {Writable} */
const arduinoIn = new Writable()
/** @type {Writable} */
const arduinoOut = new Writable()

//custom tcp middleware so can process it as a http request or i can use the socket for
//bi-directional comunication between the server and the arduino if the first byte is a '!'
tcp_server.on("connection", async (socket) => {
	console.log("[TCP] connection %s}", socket.remoteAddress)

	//handle socket error
	socket.on("error", () => {
		socket.destroy()
	})

	//wait until data recieved
	const data = await new Promise((resolve) => {
		socket.once("data", (data) => resolve(data))
	})

	//check if byte is '!'
	if (data[0] == 33) {
		HandleArduinoConnection(socket, arduinoIn, arduinoOut)
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
