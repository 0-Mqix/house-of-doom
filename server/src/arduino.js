const net = require("net")
const { Writable } = require("stream")

/**
@param {net.Socket} socket
@param {Writable} arduinoIn
@param {Writable} arduinoOut
*/
function ConnectArduino(socket, arduinoIn, arduinoOut) {
	console.log("[ARDUINO] connected from %s", socket.remoteAddress)

	socket.on("data", (data) => {
		arduinoOut.emit("data", data)
		console.log(data.toString())
		socket.write(data.toString())
	})

	arduinoIn.on("data", (data) => socket.write(data))
}

module.exports = {
	ConnectArduino
}
