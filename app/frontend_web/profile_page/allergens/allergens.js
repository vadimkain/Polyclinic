document.addEventListener('DOMContentLoaded', async function() {
    const token = localStorage.getItem('token');
    if (!token) {
        window.location.href = '../signin.html';
        return;
    }

    try {
        const response = await fetch('/api/allergens', {
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
            const allergensList = document.getElementById('allergensList');
            data.allergens.forEach(allergen => {
                const allergenItem = document.createElement('div');
                allergenItem.classList.add('allergen-item');
                allergenItem.innerHTML = `
                    <p>Name: ${allergen.name}</p>
                `;
                allergensList.appendChild(allergenItem);
            });
        } else {
            const errorData = await response.json();
            alert(errorData.message || 'Failed to fetch allergens');
            window.location.href = '../';
        }
    } catch (error) {
        console.error('Error:', error);
        alert(error);
        window.location.href = '../';
    }
});
