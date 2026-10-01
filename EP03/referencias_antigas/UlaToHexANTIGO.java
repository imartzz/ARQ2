import java.io.*;
import java.util.*;

public class UlaToHex{

    private static final Map<String, Character> OPCODES = Map.ofEntries(
            Map.entry("umL", '0'),
            Map.entry("zeroL", '1'),
            Map.entry("AonB", '2'),
            Map.entry("nAonB", '3'),
            Map.entry("AeBn", '4'),
            Map.entry("nB", '5'),
            Map.entry("nA", '6'),
            Map.entry("nAxnB", '7'),
            Map.entry("AxB", '8'),
            Map.entry("copiaA", '9'),
            Map.entry("copiaB", 'A'),
            Map.entry("AeB", 'B'),
            Map.entry("AenB", 'C'),
            Map.entry("nAeB", 'D'),
            Map.entry("AoB", 'E'),
            Map.entry("nAeBn", 'F')
    );

    public static void main(String[] args) {
        String inFile = "TESTEULA.ULA";
        String outFile = "testeula.hex";

        try {
            List<String> linhas = lerArquivo(inFile);
            List<String> instrucoes = traduz(linhas);
            salvar(outFile, instrucoes);
            System.out.println("Conversão finalizada: " + outFile);
        } catch (IOException e) {
            System.err.println("Erro: " + e.getMessage());
        }
    }

    // lê .ula e retorna as linhas
    private static List<String> lerArquivo (String nome) throws IOException {
        List<String> linhas = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(nome))) {
            String l;
            while ((l = br.readLine()) != null) {
                l = l.trim();
                if (!l.isEmpty()) {
                    linhas.add(l);
                }
            }
        }
        return linhas;
    }

    // salva as instruções convertidas no .hex
    private static void salvar (String nome, List<String> linhas) throws IOException {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(nome))) {
            for (String l : linhas) {
                bw.write(l);
                bw.newLine();
            }
        }
    }

    // traduz as linhas lidas em instruções hexadecimais
    private static List<String> traduz (List<String> linhas) {
        List<String> result = new ArrayList<>();
        String X = "0";
        String Y = "0";

        for (String l : linhas) {
            if (l.equalsIgnoreCase("inicio:") || l.equalsIgnoreCase("fim.")) continue;

            for (String parte : l.split(";")) {
                parte = parte.trim();
                if (parte.isEmpty() || !parte.contains("=")) continue;

                String[] kv = parte.split("=");
                if (kv.length != 2) continue;

                String var = kv[0].trim();
                String val = kv[1].replaceAll("[.;]", "").trim();

                switch (var.toUpperCase()) {
                    case "X" -> X = val;
                    case "Y" -> Y = val;
                    case "W" -> {
                        Character op = OPCODES.get(val);
                        if (op != null) {
                            result.add(X + Y + op);
                        } else {
                            System.err.println("Mnemonico inválido: " + val);
                        }
                    }
                }
            }
        }
        return result;
    }
}