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
                const bookedTime = formatDate(new Date(data.latest_booked_doctor.booked_time * 1000));
                const isCompleted = data.latest_booked_doctor.is_compited ? 'Yes' : 'No';
                const bookId = String(data.latest_booked_doctor.book_id);

                document.getElementById('latestBookedDoctor').innerHTML = 
                    `Book №${bookId}, Doctor: ${docName}, Appointment Time: ${bookedTime}, Completed: ${isCompleted} <a href="#" class="more-link">More...</a>`;
            }

            // Обработка последнего приёма у доктора
            if (data.latest_doctor_appointment) {
                document.getElementById('latestDoctorAppointment').textContent = 
                    `Номер записи: ${data.latest_doctor_appointment.book_id}, Время приёма: ${formatDate(new Date(data.latest_doctor_appointment.appointment_time * 1000))}, Врач: ${data.latest_doctor_appointment.doctor_name}, Жалоба: ${data.latest_doctor_appointment.complaint}`;
            }

            // Обработка последнего выписанного лекарства
            if (data.latest_prescription_drug) {
                document.getElementById('latestPrescriptionDrug').textContent = 
                    `Номер записи: ${data.latest_prescription_drug.book_id}, Врач: ${data.latest_prescription_drug.doctor_name}, Название лекарства: ${data.latest_prescription_drug.drug_name}`;
            }

            // Обработка последнего назначенного анализа
            if (data.latest_analyse_appointment) {
                document.getElementById('latestAnalyseAppointment').textContent = 
                    `Номер записи: ${data.latest_analyse_appointment.book_id}, Врач: ${data.latest_analyse_appointment.doctor_name}, Выполнено: ${data.latest_analyse_appointment.is_completed ? 'Да' : 'Нет'}`;
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

function formatDate(date) {
    const day = String(date.getDate()).padStart(2, '0');
    const month = String(date.getMonth() + 1).padStart(2, '0'); // Месяц начинается с 0
    const year = date.getFullYear();
    return `${day}.${month}.${year}`;
}
