const net = require("net")
const { Writable } = require("stream")
const { GetClicks, IncrementClicks, SaveData } = require("./database")

const NL = "\n".charCodeAt(0)
const BS = "\b".charCodeAt(0)
const CR = "\r".charCodeAt(0)

/**
@description function where the arduino conection is handled by 
adding listners and processing the incoming data 

@param {net.Socket} socket
@param {Writable} arduinoIn
@param {Writable} clientsIn
*/
async function HandleArduinoConnection(socket, arduinoIn, clientsIn) {
	console.log("[ARDUINO] connected from %s", socket.remoteAddress)

	socket.write(`set_clicks:${await GetClicks()}\n`)

	/**@type {string} */
	let line = ""

	socket.on("close", () => {
		console.log("[ARDUINO] connection closed from %s", socket.remoteAddress)
	})

	socket.on("data", (data) => {
		for (let i = 0; i < data.length; i++) {
			const char = data[i]

			//ignore backspace and \r
			if (char == BS || char == CR) {
				continue
			}

			//if new line start new line and emit and print finished line
			if (char == NL) {
				// console.log("[ARDUINO] -> " + line)

				//pass message in command handler
				MessageHandler(socket, line, clientsIn)
				line = ""
			} else {
				//add char to current line
				line += String.fromCharCode(char)
			}
		}
	})

	//add event listner to the arduino input stream
	arduinoIn.on("data", (data) => socket.write(data))
}

/**
	@description this processes the complete message
	@param {string} socket 
	@param {string} message 
	@param {Writable} clientsIn
*/
async function MessageHandler(socket, message, clientsIn) {
	if (message == "click") {
		console.log("[ARDUINO] incrementing click...")

		await IncrementClicks()
		const new_message = `set_clicks:${await GetClicks()}`

		socket.write(new_message + "\n")
		clientsIn.emit("data", new_message)

		return
	}

	if (message.startsWith("data:")) {
		data = message.replace("data:", "").split(",")

		SaveData(data[0], data[1])

		clientsIn.emit("data", "temprature:" + data[0])
		clientsIn.emit("data", "humidity:" + data[1])

		return
	}

	clientsIn.emit("data", message)
}

module.exports = {
	HandleArduinoConnection
}
