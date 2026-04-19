function toggleCSS() {
    var curr_style = document.getElementById("css_style");
    var curr_file = curr_style.getAttribute("href");
    var new_file = (curr_file === "./theme1.css") ? "./theme2.css" : "./theme1.css";
    curr_style.setAttribute("href", new_file);

    var curr_logo = document.getElementById("toggle-style-button").innerHTML;
    var new_logo = (curr_logo === "🤘") ? "👍" : "🤘";
    document.getElementById("toggle-style-button").innerHTML = new_logo;

    localStorage.setItem("theme", new_file);
    localStorage.setItem("logo", new_logo);
}

window.onload = function() {
    var stylesheet = (localStorage.getItem("theme") === null) ? "./theme1.css" : localStorage.getItem("theme");
    var style = document.getElementById("css_style");
    var logo = localStorage.getItem("logo");
    document.getElementById("toggle-style-button").innerHTML = (stylesheet === "./theme2.css") ? "🤘" : "👍";
    style.setAttribute("href", stylesheet);
}