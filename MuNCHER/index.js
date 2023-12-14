const { app, BrowserWindow, ipcMain, remote, dialog} = require("electron");

//Custom C++ implementation of fast quaternion W axis calculation
// var quat = require('.././build/Release/Quat');

//UDP library
var dgram = require('dgram');

//TCP library
var net = require('net');

//Filesystem access
const fs = require('fs');

const path = require('path');

//Command line interface
var os = require('os');

var serverStartup = true;

const UDP_IN		  = 6666;
const UDP_OUT		  = 6667;
const ROBO_IP		  = '192.168.16.99';

var connectionStatusCheck = 0;

//For valuable data (1.5 sec update or when data changed)
var slowStateUpdated = false;

//For throwaway frame based data like LED or haptic states
var fastStateUpdated = false;

//UDP Vars
var serverUDP;
var connectionAlive;

//Serial UDP
var serverSerialUDP;

function updateWindow(windowType)
{
  appWindow.webContents.send('windowType', windowType);
}

var windowModifier = 0.7
var minModifier = 0.7;
let win2;
let appWindow;

var logStream;
let date_ob = new Date();


var isConnected = 0;

function resetSuitRegs() {
	console.log("reset");
}

//Add leading 0's if needed to number
function padTo2Digits(num) {
	return num.toString().padStart(2, '0');
}


let mainWindowIsLoaded = false;
let recognitionLoaded = false;
let firstLoad = true;

function createWindow() {
	loadWindow = new BrowserWindow({
	        width: 600 * windowModifier,
	        height: 400 * windowModifier,
	        resizable: false,
	        center: true,
	        frame: false,
	        show: false,
	        alwaysOnTop: true,
	        webPreferences: {
	            nodeIntegration: true
	        }
	        
	    });

	//Create the browser window.
	appWindow = new BrowserWindow({
	    width: 1920 * windowModifier,
	    height: 1080 * windowModifier,
	    minWidth: 1920 * minModifier,
	    minHeight: 1080 * minModifier,
	    backgroundColor: "#1e202e",
	    center: true,
	    show: false,
	    webPreferences: {
	        nodeIntegration: true,
	        contextIsolation: false,
	       enableRemoteModule: true
	    }
	});


    loadWindow.loadFile("loading.html");
    

    appWindow.on('close', () =>
    {
      if(serverStartup == false)
      {
       serverUDP.close();
       serverSerialUDP.close();
      }
    })
    
    // and load the index.html of the app.
    appWindow.loadFile("./index.html");
    
    loadWindow.once('ready-to-show', () => {
				loadWindow.show();
				appWindow.once('ready-to-show', () => {
					mainWindowIsLoaded = true;
			    });
				setTimeout(next, 200);
    });
    //appWindow.webContents.openDevTools();
}

function next() {
	setTimeout(() => {
		if(mainWindowIsLoaded == true && firstLoad == true) {
			appWindow.show();
			// Wireless();
			loadWindow.close();
		}
		else {
			firstLoad = false;
		}
	}, 500);
}

function Wireless() {
	//UDP Stream Data
	serverUDP = dgram.createSocket('udp4');
	console.log('UDP Command Server sending to ' + ROBO_IP + ':' + UDP_OUT);

	//For incoming data
	udpIn = dgram.createSocket('udp4');
	//Bind UDP Stream listener to a specific port
    udpIn.bind(UDP_IN);

	udpIn.on('listening', function() {
		var address = udpIn.address();
		console.log('UDP Stream Server listening on ' + address.address + ':' + UDP_IN);
	});

	udpIn.on('message', function(message, remote) {
		//on packet recieve, convert stream to a Buffer element
		var bufferedPacket = Buffer.from(message);
		parseIncomingData(bufferedPacket.toString());
	});
}


app.on("ready", createWindow);

app.on('window-all-closed', () => {
	serverUDP.close();
	app.quit()
});

Wireless();

var udpByteArray;
function sendAndFormatUDPPacket(data) {
	//Create output byte array

let angle = data.left * -180 + data.right * 180;
let speed = Math.min(data.forward * 100 + data.left * 250 + data.right * 250, 250) - 100 * data.back - Math.min(data.back * data.left * 250 + data.back * data.right * 250 , 250);

let string = data.mode + "a" + angle + "," + speed + "," + data.soilSample + ",0,0,0,0,a";
let array = Buffer.from(string);

	serverUDP.send(array,0, array.length, UDP_OUT, ROBO_IP);
}

function parseIncomingData(bufferedPacket) {
	appWindow.webContents.send('incomingData', bufferedPacket);
	// console.log(parseFloat(bufferedPacket.slice(2)));
}

ipcMain.on("driveCommand", (event, data) => {
	sendAndFormatUDPPacket(data)
});

