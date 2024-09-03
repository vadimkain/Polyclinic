document.addEventListener('DOMContentLoaded', async function() {
    const token = localStorage.getItem('token');
    if (!token) {
        window.location.href = '/profile_page/signin.html';
        return;
    }

    try {
        const response = await fetch('/api/prescription_drugs', {
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
            const prescriptionDrugsList = document.getElementById('prescriptionDrugsList');
            prescriptionDrugsList.innerHTML = ''; // Очищаем список

            data.prescriptionDrugsList.forEach(drug => {
                const drugItem = document.createElement('div');
                drugItem.classList.add('prescription-drug-item');
                drugItem.innerHTML = `
                    <p>Book №${drug.appointment_id}</p>
                    <p>Doctor: ${drug.doc_name} ${drug.doc_middle_name} ${drug.doc_surname}</p>
                    <p>Drug Name: ${drug.drug_name}</p>
                `;
                prescriptionDrugsList.appendChild(drugItem);
            });
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch prescription drugs');
            window.location.href = '/profile_page/';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = '/profile_page/';
    }
});
