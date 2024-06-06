document.addEventListener('DOMContentLoaded', function() {
    const registerForm = document.getElementById('registerForm');
    const signinForm = document.getElementById('signinForm');

    if (registerForm) {
        registerForm.addEventListener('submit', function(event) {
            event.preventDefault();
            localStorage.setItem('isLoggedIn', 'true');
            window.location.href = 'profile.html';
        });
    }

    if (signinForm) {
        signinForm.addEventListener('submit', function(event) {
            event.preventDefault();
            localStorage.setItem('isLoggedIn', 'true');
            window.location.href = 'profile.html';
        });
    }
});

function logout() {
    // Логика выхода из учетной записи
    // Например, очистка токена авторизации и перенаправление на страницу входа
    alert('You have been logged out');
    localStorage.setItem('isLoggedIn', 'false');
    window.location.href = 'signin.html';
}
