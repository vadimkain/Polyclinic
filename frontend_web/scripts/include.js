document.addEventListener("DOMContentLoaded", function() {
    var includes = document.querySelectorAll('[w3-include-html]');
    includes.forEach(function(include) {
        var file = include.getAttribute('w3-include-html');
        fetch(file)
            .then(response => {
                if (!response.ok) throw new Error('Network response was not ok');
                return response.text();
            })
            .then(data => include.outerHTML = data)
            .catch(error => console.error('There was a problem with the fetch operation:', error));
    });
});
