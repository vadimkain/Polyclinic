document.addEventListener('DOMContentLoaded', async function() {
    const token = localStorage.getItem('token');
    if (!token) {
        window.location.href = '../signin.html';
        return;
    }

    try {
        const response = await fetch('/api/analyses', {
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
            const analysesList = document.getElementById('analysesList');
            data.analyses.forEach(analysis => {
                const analysisItem = document.createElement('div');
                analysisItem.classList.add('analysis-item');
                analysisItem.innerHTML = `
                    <p>Book №${analysis.appointment_id}</p>
                    <p>Doctor: ${analysis.doc_name} ${analysis.doc_middle_name} ${analysis.doc_surname}</p>
                    <p>Analyse: ${analysis.analyse_name}</p>
                    <p>Completed: ${analysis.is_completed ? 'Yes' : 'No'}</p>
                `;
                analysesList.appendChild(analysisItem);
            });
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch analyses');
            window.location.href = '../';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = '../';
    }
});
