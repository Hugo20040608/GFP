import './App.css';

function App() {
  // fetch API
  fetch('http://localhost:8888')
  .then(response => response.text())  // 轉換響應為 text
  .then(data => console.log(data))    // 輸出數據
  .catch(error => console.error('Error:', error));  // 處理任何錯誤

  return (
    <div className="App">
      <header className="App-header">
        <h1>Hello World</h1>
      </header>
    </div>
  );
}

export default App;
