document.addEventListener('DOMContentLoaded', function () {
	document.getElementById('logout-link').addEventListener('click', function (event) {
		event.preventDefault();

		fetch('/logout', {
			method: 'POST',
			headers: {
				'Content-Type': 'application/json'
			},
			credentials: 'same-origin'
		})
			.then(response => {
				if (response.ok) {
					window.location.href = '/login';
				} else {
					return response.json().then(data => {
						toastr.error(data.message || 'Ошибка выхода!');
					});
				}
			})
			.catch(error => {
				toastr.error('Ошибка выхода!');
			});
	});
});
