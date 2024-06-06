document.addEventListener('DOMContentLoaded', function() {
    const isLoggedIn = localStorage.getItem('isLoggedIn');

    if (isLoggedIn === 'true') {
        window.location.href = '/profile_page/profile.html';
    } else {
        window.location.href = 'signin.html';
    }
});
