document.addEventListener("DOMContentLoaded", bindElems);

function bindElems() {
    var navLinks = document.getElementsByClassName("nav-link");
    for (var lnk in navLinks) {
        lnk.addEventListener("mouseover", makeActive);
    }

}

function makeActive(event) {
    event
}

