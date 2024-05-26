// bars/topbar.js
document.addEventListener('DOMContentLoaded', function() {
    const container = document.getElementById('topbar-container');
    fetch('topbar.html')
        .then(response => response.text())
        .then(html => container.innerHTML = html);
});
