const electron = require('electron');
const remote = require("electron").remote;

// Import the ipcRenderer Module from Electron
const ipcRenderer = electron.ipcRenderer; 

							//Forward, backward, left, right	
let driveArray = {
		forward: 0,
		back: 0,
		left: 0,
		right: 0,
		mode: "a",
		soilSample: 0,
};

function updateSoilSampler(sample) {
	driveArray.soilSample = sample;
}


function changeModeAuto(){

	document.getElementById('autoMode').classList.add('modeSelected');
	document.getElementById('WASDMode').classList.remove('modeSelected');
	document.getElementById('RCMode').classList.remove('modeSelected');
	driveArray.mode = "b";
}
function changeModeWASD(){
	document.getElementById('autoMode').classList.remove('modeSelected');
	document.getElementById('WASDMode').classList.add('modeSelected');
	document.getElementById('RCMode').classList.remove('modeSelected');
	driveArray.mode = "a";
}
function changeModeInf(){
	document.getElementById('autoMode').classList.remove('modeSelected');
	document.getElementById('WASDMode').classList.remove('modeSelected');
	document.getElementById('RCMode').classList.add('modeSelected');
	driveArray.mode = "c";
}

autoEnabled = 0;
function toggleAutoRun() {
	autoEnabled = autoEnabled ? false : true; 
	if(autoEnabled) {
		document.getElementById('autoToggle').classList.remove('autoStop');
		document.getElementById('autoToggle').classList.add('autoRun');
		document.getElementById('autoToggle').innerHTML = "Auto Running";
	}
	else {
		document.getElementById('autoToggle').classList.add('autoStop');
		document.getElementById('autoToggle').classList.remove('autoRun');
		document.getElementById('autoToggle').innerHTML = "Auto Stopped";
	}

}

function loadAuto() {
	console.log('No');
}

document.addEventListener("keydown", event => {
	switch (event.key) 
	{
		//Get out of fullscreen mode
		case "Escape":
			if (remote.getCurrentWindow().isFullScreen())
			{
				remote.getCurrentWindow().setFullScreen(false);
			}
			break;

		case "w":
			driveArray.forward = 1;
			break;

		case "a":
			driveArray.left = 1;
			break;

		case "s":
			driveArray.back = 1;
			break;

		case "d":
			driveArray.right = 1;
			break;
		}
});

document.addEventListener("keyup", event => {

	switch (event.key) 
	{
		case "w":
			driveArray.forward = 0;
			break;

		case "a":
			driveArray.left = 0;
			break;

		case "s":
			driveArray.back = 0;
			break;

		case "d":
			driveArray.right = 0;
			break;
		}
});

function sendDriveCommand() {
		ipcRenderer.send("driveCommand", driveArray);
}


function init(){
 map = new OpenLayers.Map("map");
var mapnik         = new OpenLayers.Layer.OSM();
    var lat            = 42.293397;
    var lon            = -71.263934;
    var zoom           = 16;

    var fromProjection = new OpenLayers.Projection("EPSG:4326");   // Transform from WGS 1984
    var toProjection   = new OpenLayers.Projection("EPSG:900913"); // to Spherical Mercator Projection
    var position       = new OpenLayers.LonLat(lon, lat).transform( fromProjection, toProjection);

map.addLayer(mapnik);
map.setCenter(position, zoom );
setInterval(sendDriveCommand, 50);
}

ipcRenderer.on('incomingData', (event, data) => {
		document.getElementById("settings-voltage-grid").innerHTML = parseFloat(data.slice(2));
	});


