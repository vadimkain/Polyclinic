document.addEventListener('DOMContentLoaded', async function() {
    const token = localStorage.getItem('token');
    if (!token) {
        window.location.href = '/profile_page/signin.html';
        return;
    }

    try {
        const response = await fetch('/api/appointments', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': token
            },
            credentials: 'include',
            body: JSON.stringify({ token: parseInt(token) })
        });

        if (response.ok) {
            const data = await response.json();
            const appointmentsList = document.getElementById('appointmentsList');
            appointmentsList.innerHTML = ''; // Очищаем список

            data.appointmentsList.forEach(appointment => {
                const appointmentItem = document.createElement('div');
                appointmentItem.classList.add('appointment-item');
                appointmentItem.innerHTML = `
                    <p>Book №${appointment.book_id}</p>
                    <p>Doctor: ${appointment.doc_name} ${appointment.doc_middle_name} ${appointment.doc_surname}</p>
                    <p>Appointment Time: ${formatDateTime(new Date(appointment.appointment_time * 1000))}</p>
                    <p>Complaint: ${appointment.complaint}</p>
                `;
                appointmentsList.appendChild(appointmentItem);
            });
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch appointments');
            window.location.href = '/profile_page/';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = '/profile_page/';
    }
});

function formatDateTime(date) {
    const day = String(date.getDate()).padStart(2, '0');
    const month = String(date.getMonth() + 1).padStart(2, '0'); // Месяц начинается с 0
    const year = date.getFullYear();
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    const seconds = String(date.getSeconds()).padStart(2, '0');
    return `${day}.${month}.${year} ${hours}:${minutes}:${seconds}`;
}
