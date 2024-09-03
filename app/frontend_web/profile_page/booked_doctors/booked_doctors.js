document.addEventListener('DOMContentLoaded', async function() {
    const isLoggedIn = localStorage.getItem('isLoggedIn');
    const token = localStorage.getItem('token');

    if (isLoggedIn !== 'true' || !token) {
        window.location.href = '../profile_page/signin.html';
        return;
    }

    try {
        const response = await fetch('/api/booked_doctors', {
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
            const bookedDoctorsList = document.getElementById('bookedDoctorsList');
            bookedDoctorsList.innerHTML = ''; // Очищаем список

            data.forEach(doctor => {
                const docName = `${doctor.doc_name} ${doctor.doc_middle_name} ${doctor.doc_surname}`;
                const bookedTime = formatDateTime(new Date(doctor.booked_time * 1000));
                const isCompleted = doctor.is_completed ? 'Yes' : 'No';
                const bookId = doctor.book_id;

                const listItem = document.createElement('li');
                listItem.innerHTML = `Book №${bookId}, Doctor: ${docName}, Appointment Time: ${bookedTime}, Completed: ${isCompleted}`;
                bookedDoctorsList.appendChild(listItem);
            });
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch booked doctors data');
            window.location.href = '../profile_page/signin.html';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = '../profile_page/signin.html';
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
