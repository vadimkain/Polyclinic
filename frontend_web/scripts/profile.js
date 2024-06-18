document.addEventListener('DOMContentLoaded', async function() {
    const isLoggedIn = localStorage.getItem('isLoggedIn');
    const token = localStorage.getItem('token');

    if (isLoggedIn !== 'true' || !token) {
        window.location.href = 'signin.html';
        return;
    }

    try {
        const response = await fetch('/api/user', {
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

            // Заголовок профиля
            document.getElementById('profileNameHeader').textContent = `${data.name} ${data.middle_name} ${data.surname}`;
            document.getElementById('profileEmailHeader').textContent = data.email;

            // Личная информация
            document.getElementById('profileName').textContent = `${data.name} ${data.middle_name} ${data.surname}`;
            document.getElementById('profileEmail').textContent = data.email;
            document.getElementById('profilePhoneNumbers').textContent = data.phone_numbers.join(', ');

            // Обработка последнего аллергена
            if (data.latest_allergen) {
                document.getElementById('latestAllergen').textContent = data.latest_allergen.name;
            }

            // Обработка последней записи к врачу
            if (data.latest_booked_doctor) {
                const docName = `${data.latest_booked_doctor.doc_name} ${data.latest_booked_doctor.doc_middle_name} ${data.latest_booked_doctor.doc_surname}`;
                const bookedTime = formatDateTime(new Date(data.latest_booked_doctor.booked_time * 1000));
                const isCompleted = data.latest_booked_doctor.is_compited ? 'Yes' : 'No';
                const bookId = data.latest_booked_doctor.book_id;

                document.getElementById('latestBookedDoctor').innerHTML = 
                    `Book №${bookId}, Doctor: ${docName}, Appointment Time: ${bookedTime}, Completed: ${isCompleted} <a href="#" class="more-link">More...</a>`;
            }

            // Обработка последнего приёма у доктора
            if (data.last_appointment_info) {
                const docName = `${data.last_appointment_info.doc_name} ${data.last_appointment_info.doc_middle_name} ${data.last_appointment_info.doc_surname}`;
                const appointmentTime = formatDateTime(new Date(data.last_appointment_info.appointment_time * 1000));

                document.getElementById('latestDoctorAppointment').innerHTML = 
                    `Book №${data.last_appointment_info.book_id}, Doctor: ${docName}, Appointment Time: ${appointmentTime}, Complaint: ${data.last_appointment_info.complaint} <a href="#" class="more-link">More...</a>`;
            }

            // Обработка последнего выписанного лекарства
            if (data.last_descriptioned_drug_info) {
                const docName = `${data.last_descriptioned_drug_info.doc_name} ${data.last_descriptioned_drug_info.doc_middle_name} ${data.last_descriptioned_drug_info.doc_surname}`;

                document.getElementById('latestPrescriptionDrug').innerHTML = 
                    `Book №${data.last_descriptioned_drug_info.appointment_id}, Doctor: ${docName}, Drug Name: ${data.last_descriptioned_drug_info.drug_name} <a href="#" class="more-link">More...</a>`;
            }

            // Обработка последнего назначенного анализа
            if (data.last_analyse_appointment_info) {
                const docName = `${data.last_analyse_appointment_info.doc_name} ${data.last_analyse_appointment_info.doc_middle_name} ${data.last_analyse_appointment_info.doc_surname}`;
                const isCompleted = data.last_analyse_appointment_info.is_completed ? 'Yes' : 'No';

                document.getElementById('latestAnalyseAppointment').innerHTML = 
                    `Book №${data.last_analyse_appointment_info.appointment_id}, Doctor: ${docName}, Analyse: ${data.last_analyse_appointment_info.analyse_name}, Completed: ${isCompleted} <a href="#" class="more-link">More...</a>`;
            }
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch profile data');
            window.location.href = 'signin.html';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = 'signin.html';
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
