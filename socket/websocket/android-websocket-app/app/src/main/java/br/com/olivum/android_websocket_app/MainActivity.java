package br.com.olivum.android_websocket_app;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import com.koushikdutta.async.AsyncServer;
import com.koushikdutta.async.callback.CompletedCallback;
import com.koushikdutta.async.http.WebSocket;
import com.koushikdutta.async.http.server.AsyncHttpServer;
import com.koushikdutta.async.http.server.AsyncHttpServerRequest;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private List<WebSocket> webSockets = new ArrayList<WebSocket>();
    private int port = 8081;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);


        Handler handler = new Handler();

        handler.post(() -> {
            connectToServer();
        });
    }

    private void connectToServer() {
        // Websocket server

        AsyncHttpServer httpServer = new AsyncHttpServer();

        httpServer.listen(AsyncServer.getDefault(), port);

        httpServer.websocket("/live", new AsyncHttpServer.WebSocketRequestCallback() {
            @Override
            public void onConnected(final WebSocket webSocket, AsyncHttpServerRequest request) {
                webSockets.add(webSocket);

                webSocket.send("Message from Android");

                //Use this to clean up any references to your websocket
                webSocket.setClosedCallback(new CompletedCallback() {
                    @Override
                    public void onCompleted(Exception ex) {
                        try {
                            if (ex != null) {
                                Log.e("WebSocket", "An error occurred", ex);
                            }
                        }
                        finally {
                            webSockets.remove(webSocket);
                        }
                    }
                });

                webSocket.setStringCallback(new WebSocket.StringCallback() {
                    @Override
                    public void onStringAvailable(String s) {
                        if ("Hello Server".equals(s)) {
                            webSocket.send("Welcome Client!");
                        }
                    }
                });

            }
        });

        //..Sometime later, broadcast!
        for (WebSocket socket : webSockets) {
            socket.send("Fireball!");
        }
    }
}