document.addEventListener('DOMContentLoaded', function() {
    const registerForm = document.getElementById('registerForm');
    const signinForm = document.getElementById('signinForm');
    const logoutButton = document.getElementById('logoutButton');

    if (signinForm) {
        signinForm.addEventListener('submit', async (event) => {
            event.preventDefault();
            const email = document.getElementById('email').value;
            const password = document.getElementById('password').value;

            try {
                const response = await fetch('/api/login', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ email, password }),
                    credentials: 'include'
                });

                const data = await response.json();
                if (response.ok && data.is_login_success) {
                    localStorage.setItem('isLoggedIn', 'true');
                    localStorage.setItem('id', data.id);
                    localStorage.setItem('name', data.name);
                    localStorage.setItem('surname', data.surname);
                    localStorage.setItem('middle_name', data.middle_name);
                    localStorage.setItem('email', data.email);
                    localStorage.setItem('role', data.role); // Зберігання ролі у локальному сховищі
                    localStorage.setItem('phone_numbers', JSON.stringify(data.phone_numbers));
                    window.location.href = 'profile.html';
                } else {
                    alert(data.message || 'Login failed');
                }
            } catch (error) {
                console.error('Error:', error);
            }
        });
    }

    if (registerForm) {
        registerForm.addEventListener('submit', async (event) => {
            event.preventDefault();
            const fullname = document.getElementById('fullname').value;
            const email = document.getElementById('email').value;
            const password = document.getElementById('password').value;
            const confirmpassword = document.getElementById('confirmpassword').value;
            const role_id = 1; // тимчасово задаємо роль ID як 1

            if (password !== confirmpassword) {
                alert('Passwords do not match');
                return;
            }

            const nameParts = fullname.trim().split(' ');
            const name = nameParts[0];
            const surname = nameParts[1] || '';
            const middle_name = nameParts[2] || '';

            try {
                const response = await fetch('/api/register', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ name, surname, middle_name, email, password, role_id }),
                    credentials: 'include'
                });

                const data = await response.json();
                if (response.ok && data.is_success) {
                    localStorage.setItem('isLoggedIn', 'true');
                    window.location.href = 'profile.html';
                } else {
                    alert('Registration failed');
                }
            } catch (error) {
                console.error('Error:', error);
            }
        });
    }

    if (logoutButton) {
        logoutButton.addEventListener('click', () => {
            localStorage.clear(); // Видаляємо всі дані
            window.location.href = 'signin.html';
        });
    }
});
