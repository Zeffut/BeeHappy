//Batterie Dynamic Principal
const batteryHTML = document.getElementById('battery');
function updateBattery() {
    fetch(`https://humble-mantis-evident.ngrok-free.app/api/get/battery/1`)
        .then(response => response.json())
        .then(data => {
            console.log('Données récupérées depuis le serveur :', data);
            batteryHTML.innerHTML = data[0]['battery'] + '%';
    })
        .catch(error => {
            console.log('Erreur lors de la récupération des données :', error);
            batteryHTML.innerHTML = "indisponible";
    });
}

const poidsHTML = document.getElementById('poids');
function updatePoids() {
    fetch(`https://humble-mantis-evident.ngrok-free.app/api/get/weight/1`)
        .then(response => response.json())
        .then(data => {
            console.log('Données récupérées depuis le serveur :', data);
            poidsHTML.innerHTML = data[0]['weight'] + 'kg';
        })
        .catch(error => {
            console.error('Erreur lors de la récupération des données :', error);
            poidsHTML.innerHTML = "indisponible";
    });
}


//Map
const latitude = 0;
const longitude = 0;

function updateMapAndCoordinates() {
    // Effectuer la requête HTTP
    fetch('https://humble-mantis-evident.ngrok-free.app/api/get/coords/1')
    .then(response => response.json())
    .then(data => {
        // Mettre à jour les variables avec les données de la réponse
        const latitude = data[0].coordsLat;
        const longitude = data[0].coordsLng;

        // Créer la carte Leaflet
        var map = L.map('map').setView([latitude, longitude], 13);

        // Ajouter la couche de tuiles OpenStreetMap à la carte
        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
            attribution: ''
        }).addTo(map);

        // Ajouter un marqueur à la position donnée sur la carte
        L.marker([latitude, longitude]).addTo(map)
            .bindPopup('Ruche')
            .openPopup();

        // Appeler la fonction getAddress avec les coordonnées
        getAddress(latitude, longitude);

        // Mettre à jour les éléments HTML avec les coordonnées
        document.getElementById("latitude").innerText = latitude;
        document.getElementById("longitude").innerText = longitude;
    })
    .catch(error => {
        console.error('Erreur lors de la récupération des coordonnées:', error);
    });
}


function getAddress(latitude, longitude) {
    var xhr = new XMLHttpRequest();
    var url = `https://nominatim.openstreetmap.org/reverse?format=json&lat=${latitude}&lon=${longitude}&zoom=18&addressdetails=1`;

    xhr.onreadystatechange = function() {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                var response = JSON.parse(xhr.responseText);
                var address = response.display_name;
                document.getElementById("address").innerText = address;
            } else {
                console.error('Erreur lors de la requête : ' + xhr.status);
            }
        }
    };

    xhr.open('GET', url, true);
    xhr.send();
}

// Fonction pour récupérer les données depuis l'API
async function fetchData() {
    try {
        const response = await fetch('https://humble-mantis-evident.ngrok-free.app/api/get/battery/10');
        const data = await response.json();
        return data;
    } catch (error) {
        console.error('Une erreur est survenue lors de la récupération des données :', error);
    }
}

// Fonction pour générer le tableau HTML à partir des données
async function generateTable() {
    const data = await fetchData();

    if (!data) return;

    const tableBody = document.getElementById('table-body');

    data.forEach(item => {
        const row = document.createElement('tr');
        const batteryCell = document.createElement('td');
        const dateTimeCell = document.createElement('td');

        batteryCell.textContent = item.battery;
        dateTimeCell.textContent = new Date(item.messageDateTime).toLocaleString();

        row.appendChild(batteryCell);
        row.appendChild(dateTimeCell);
        tableBody.appendChild(row);
    });
}

window.onload = generateTable(), updateBattery(), updatePoids(), updateMapAndCoordinates();