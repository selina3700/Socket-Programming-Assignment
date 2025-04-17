<h1>COMP 1027 - Socket Programming Coursework</h1>
<h3>Task 1</h3>
  <p>Modify the original server code so that its output alphanumeric message is in reverse order to the client’s input message. Please note that all alphabets MUST be converted to uppercase at the server side. 
  Your input (in the client side) can be a mixture of upper and lowercase.</p>
<h3>Task 2</h3>
  <p>Improve the server code (from Server1.c) to handle multiple requests (instead of one) from the same session. For each new connection, the server window should display:</p>
  <ul>
    <li>The IP address of the client</li>
    <li>The port number targeted by the client’s request.</li>
  </ul>
  <p>For each message received, the server should display the length of the message. Your codes should also handle errors should the client send over unique characters as a message. An error message should be returned from the server to the client to 
    inform the client of these errors and to key in an alphanumeric format message only.</p>
<h3>Task 3</h3>
  <p>Modify the server code (from Server2.c) to handle specific COMMANDS sent from the client. A particular command, in this task, is “Date”.</p>
  <p>Type “Date” in the client, which will be sent to the server, the server should respond with the Current Day, Date and Time (in its Current Time-Zone, e.g., KL).</p>
  <ul>
    <li>This should be presented as a single line, that is terminated with a carriage return (ASCII code 13) and line feed (ASCII code 10).</li>
    <li>Example of output: Wed Nov 30 17:00:54 2024 GMT + 8</li>
  </ul>
<h3>Task 4</h3>
  <p>Modify the server code (from Server3.c) to handle additional commands sent from the client. A particular command, in this task, is “Time” and a code of a time-zone.</p>
  <p>Typing “Time” in the client, which will be sent to the server, the server should respond with only the Current Time (in the Current Time-Zone, which is in “Malaysia” time)</p>
<h3>Task 5</h3>
  <p>Create two clients and one server that allows both clients to communicate through a server in real-time. Both clients should be able to communicate with infinite number of text (e.g., your WhatsApp Chat Group) where one client send text to the server and the other client sees the text from the first one on the screen.</p>
  <p>Requirements:</p>
  <ol>
    <li>Server:</li>
    <ul>
      <li>Listen for and accept connections from 2 clients.</li>
      <li>Forward messages between clients.</li>
      <li>Handle multiple clients using threads.</li>
    </ul>
    <li>Client:</li>
      <ul>
        <li>Connect to the Server.</li>
        <li>Send and receive messages in real time.</li>
        <li>Display received messages clearly.</li>
        <li>Allow continuous messaging until the user decides to exit.</li>
        <li>Implement commands like “Exit Server” to disconnect.</li>
      </ul>
  </ol>
