document.addEventListener('DOMContentLoaded', () => {
    const activityLog = document.getElementById('activity-log'); // Your table ID

    const fetchLog = async () => {
        try {
            const response = await fetch('live_data.json'); // Fetch local JSON file
            const data = await response.json();

            // Clear existing rows
            activityLog.innerHTML = '';

            // Populate the table with log data
            data.forEach(entry => {
                const row = document.createElement('tr');
                row.innerHTML = `
                    <td>${entry.Date}</td>
                    <td>${entry.Time}</td>
                    <td>${entry.Latitude}, ${entry.Longitude}</td>
                    <td>${entry.State}</td>
                `;
                activityLog.appendChild(row);
            });
        } catch (error) {
            console.error("Error fetching live data:", error);
        }
    };

    // Fetch data on page load
    fetchLog();

    // Optionally, refresh data every 30 seconds
    setInterval(fetchLog, 30000);
});
