const responseBox = document.getElementById("response");
const listBox = document.getElementById("list");

document.getElementById("btnList").onclick = loadList;

document.getElementById("btnCapture").onclick = capture;

// Handler event onclick btnList, crida API endpoint "list" que retorna una llista de les imatges existents
async function loadList()
{
    responseBox.textContent = "Carregant...";

    try
    {
        const r = await fetch("/list");
        const files = await r.json();

        listBox.innerHTML = "";

        // Crear un enllaç per cada imatge que crida al API endpoint "download" amb el nom de la imatge com argument
        files.forEach(file =>
        {
            const link = document.createElement("a");

            link.className = "file";
            link.textContent = file;

            link.href = "/download?file=" + encodeURIComponent(file);

            // Obrir a una pestanya nova
            link.target = "_blank";
            link.rel = "noopener noreferrer";

            listBox.appendChild(link);
        });

        responseBox.textContent =
            files.length + " fitxers trobats.";
    }
    catch(e)
    {
        responseBox.textContent =
            "Error carregant la llista.";
    }
}

// Handler event onclick a btnCapture, crida al API endpoint capture que captura una imatge
async function capture()
{
    responseBox.textContent = "Capturant...";

    try
    {
        const r = await fetch("/capture");

        const text = await r.text();

        // escrire resultat correcte i text resposta api (nom imatge)
        responseBox.textContent =
            "Captura realitzada: " + text;
    }
    catch(e)
    {
        responseBox.textContent =
            "Error durant la captura.";
    }
}
