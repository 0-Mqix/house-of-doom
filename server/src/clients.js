const net = require("net")
const WebSocket = require("ws")
const { Writable } = require("stream")

/**
@param {WebSocket.Server<WebSocket.WebSocket>} ws
@param {Writable} arduinoIn
@param {Writable} arduinoOut
*/

function StartClientHandler(ws, arduinoIn, arduinoOut) {
	ws.on("connection", (client) => {
		console.log("client connected")

		// handle incoming messages from the client
		client.on("message", (message) => {
			console.log(`received message: ${message}`)

			// send a message back to the client
			client.send(`you said: ${message}`)

			//send event to arduino handler
			arduinoIn.emit("data", message)
		})

		//recieve message from arduino handler
		arduinoOut.on("data", (data) => {
			client.send(data.toString())
		})

		// handle client disconnection
		client.on("close", () => {
			console.log("client disconnected")
		})
	})
}

module.exports = {
	StartClientHandler
}
