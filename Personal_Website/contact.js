document.addEventListener("DOMContentLoaded", bindElems);

function bindElems() {
    var carouselItems = document.getElementsByClassName("carousel-item");

    for (var lnk in navLinks) {
        lnk.addEventListener("mouseover", makeActive);
    }

}

function validateEmail(email) {
    var re = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
    return re.test(String(email).toLowerCase());
}

function makeActive(event) {
    event
}

