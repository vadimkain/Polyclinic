document.addEventListener('DOMContentLoaded', async function() {
    const isLoggedIn = localStorage.getItem('isLoggedIn');
    const token = localStorage.getItem('token');

    if (isLoggedIn !== 'true' || !token) {
        window.location.href = '/profile_page/signin.html';
        return;
    }

    try {
        const response = await fetch('/api/doctors', {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': token
            }
        });

        if (response.ok) {
            const data = await response.json();
            const doctorSelect = document.getElementById('doctorSelect');
            data.doctors.forEach(doctor => {
                const option = document.createElement('option');
                option.value = doctor.id;
                option.textContent = `${doctor.name} ${doctor.surname}`;
                doctorSelect.appendChild(option);
            });
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch doctors list');
            window.location.href = '/profile_page/';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = '/profile_page/';
    }

    const bookDoctorForm = document.getElementById('bookDoctorForm');
    bookDoctorForm.addEventListener('submit', async function(event) {
        event.preventDefault();

        const doctorId = document.getElementById('doctorSelect').value;
        const appointmentTime = document.getElementById('appointmentTime').value;

        try {
            const response = await fetch('/api/book_doctor', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': token
                },
                body: JSON.stringify({ doctorId, appointmentTime })
            });

            if (response.ok) {
                alert('Appointment booked successfully!');
                window.location.href = '/profile_page/';
            } else {
                const errorData = await response.json();
                alert(errorData.message || 'Failed to book appointment');
            }
        } catch (error) {
            console.error('Error:', error);
            alert(error);
        }
    });
});
