const net = require("net")
const WebSocket = require("ws")
const { Writable } = require("stream")
const crypto = require("crypto")

/**
@param {WebSocket.Server<WebSocket.WebSocket>} ws
@param {Writable} arduinoIn
@param {Writable} clientsIn
*/
function StartClientHandler(ws, arduinoIn, clientsIn) {
	/** @type {Map<String, WebSocket.WebSocket>} */
	const clients = new Map()

	//send all clients the message from the clientsIn Stream
	clientsIn.on("data", (data) => {
		clients.forEach((c) => {
			c.send(data)
		})
	})

	ws.on("connection", (client) => {
		const id = crypto.randomUUID()
		//add client to map with a uuid
		clients.set(id, client)

		// handle incoming messages from the client
		client.on("message", (message) => {
			//send event to arduino handler
			arduinoIn.emit("data", message + "\n")
		})

		//handle client disconnection
		client.on("close", () => {
			console.log("[WS] [CLIENT] disconnected")
			clients.delete(id)
		})
	})
}

module.exports = {
	StartClientHandler
}
