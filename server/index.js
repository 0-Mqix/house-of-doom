const express = require("express")
const http = require("http")
const net = require("net")

const WebSocket = require("ws")
const { Writable } = require("stream")

const { HandleArduinoConnection } = require("./src/arduino.js")
const { StartClientHandler } = require("./src/clients.js")
const { GetClicks } = require("./src/database.js")

const app = express()
const http_server = http.createServer(app)
const tcp_server = net.createServer()
const ws = new WebSocket.Server({ server: http_server })

//a Writable is an object that is a stream where you can
//attach
/** @type {Writable} */
const arduinoIn = new Writable()
/** @type {Writable} */
const clientsIn = new Writable()

//custom tcp middleware so can process it as a http request or i can use the socket for
//bi-directional comunication between the server and the arduino if the first byte is a '!'
tcp_server.on("connection", async (socket) => {
	console.log("[TCP] connection %s", socket.remoteAddress)

	//handle socket error
	socket.on("error", (err) => {
		console.log(
			"[ARDUINO] connection error %s from %s",
			err.message,
			socket.remoteAddress
		)
		socket.destroy()
	})

	//wait until data recieved
	const data = await new Promise((resolve) => {
		socket.once("data", (data) => resolve(data))
	})

	//check if byte is '!'
	if (data[0] == 33) {
		HandleArduinoConnection(socket, arduinoIn, clientsIn)
		return
	}

	//resume http
	http_server.emit("connection", socket)
	socket.emit("data", data)
})

//start the websocket handler
StartClientHandler(ws, arduinoIn, clientsIn)

//serve all files in public folder
app.use(express.static("public"))

// set the view engine to ejs
app.set("view engine", "ejs")
// use res.render to load up an ejs view file

app.get("/", async (req, res) => {
	res.render("index", {
		temprature: 1,
		humidity: 2,
		clicks: await GetClicks()
	})
})

//listern on port ?
tcp_server.listen(process.env.PORT, () => {
	console.log("Server is listening on port 80")
})
