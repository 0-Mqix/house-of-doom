require("dotenv").config()
const mysql = require("mysql2/promise")

const pool = mysql.createPool(process.env.DATABASE_URL)

/** @returns {Promise<void>} */
async function IncrementClicks() {
	try {
		await pool.query(
			"UPDATE CurrentValues SET value = CASE WHEN value >= 9999 THEN 0 ELSE value + 1 END WHERE `key` = 'clicks'"
		)
	} catch (error) {
		console.log("[SQL] Increment Clicks %s", error.message)
	}
}

/** @returns {Promise<number>} */
async function GetClicks() {
	try {
		const [rows] = await pool.query(
			"SELECT value FROM CurrentValues WHERE `key` =  'clicks'"
		)
		return rows[0].value
	} catch (error) {
		console.log("[SQL] GetClicks %s", error.message)
	}
}

//5m cooldown on saving data to the database becase its unnecessary
const SEND_COOLDOWN = 1000 * 60 * 5
let last_send_time = 0

async function SaveData(temprature, humidity) {
	if (last_send_time != 0 || last_send_time > Date.now() - SEND_COOLDOWN) {
		return
	}

	// define the data to be inserted
	const data = {
		temprature: temprature,
		humidity: humidity,
		time: new Date()
	}

	try {
		console.log(`[SQL] saving data...`)
		await pool.execute(
			"INSERT INTO `Data` (`temprature`, `humidity`, `time`) VALUES (?, ?, ?)",
			[data.temprature, data.humidity, data.time]
		)
		last_send_time = Date.now()
	} catch (error) {
		console.log("[SQL] SaveData %s", error.message)
	}
}

module.exports = {
	pool,
	GetClicks,
	IncrementClicks,
	SaveData
}
