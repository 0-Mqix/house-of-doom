const net = require("net")
const { Writable } = require("stream")

const CR = "\r".charCodeAt(0)
const NL = "\n".charCodeAt(0)
const BS = "\b".charCodeAt(0)

/**
@param {net.Socket} socket
@param {Writable} arduinoIn
@param {Writable} arduinoOut
*/
function ConnectArduino(socket, arduinoIn, arduinoOut) {
	console.log("[ARDUINO] connected from %s", socket.remoteAddress)

	/**@type {string} */
	let line = ""

	socket.on("data", (data) => {
		data.forEach((char) => {
			//ignore backspace
			if (char == BS) {
				return
			}

			//add char to current line
			line += String.fromCharCode(char)

			//if new line start new line and emit and print finished line
			if (char == NL) {
				console.log(
					"[ARDUINO] -> " +
						//this is to print \r and \n without it affecting the console output
						line.replace("\r", "\\r").replace("\n", "\\n")
				)

				//emit event to the output stream
				arduinoOut.emit("data", line)
				line = ""
			}
		})
	})

	//add event listner to the arduino input stream
	arduinoIn.on("data", (data) => socket.write(data))
}

module.exports = {
	ConnectArduino
}
